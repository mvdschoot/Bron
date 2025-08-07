#ifndef _CONTAINERSSTORAGE_HEADER__
#define _CONTAINERSSTORAGE_HEADER__

#include <unordered_map>
#include <optional>
#include <ranges>
#include <list>

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"
#include "Handle.h"

namespace Steve
{
	class ArenaContainer
	{
	public:
		ArenaContainer(size_t initial_size = 1024, float hole_threshold = 0.1f) : mStorageSize(initial_size), mFragThreshold(hole_threshold), mFragHoleSize(0)
		{
			mStorage = (u8*)calloc(mStorageSize, 1);
			mStorageFreePtr = mStorage;
			mStorageContent.reserve(mStorageSize);
		}

		~ArenaContainer()
		{
			free(mStorage);
		}

		// Gets ownership of element
		template<typename T>
		T* Insert(const UUID& uuid, T&& element)
		{
			size_t size = sizeof(T);

			if (IsFullAfter(size))
				ResizeIfNeeded();

			T* ptr = new(mStorageFreePtr) T(element);

			mStorageContent.insert({ uuid, {(u8*)ptr, size } });
			mStorageFreePtr += size;

			return ptr;
		}

		// Gets ownership of object
		u8* InsertExternal(const UUID& uuid, usize size)
		{
			if (IsFullAfter(size))
				ResizeIfNeeded();

			mStorageContent.insert({ uuid, { mStorageFreePtr, size } });
			mStorageFreePtr += size;

			return mStorageFreePtr - size;
		}

		// Calling function takes ownership
		template<typename T>
		T&& Remove(const UUID& element)
		{
			const auto it = mStorageContent.find(element);
			T&& to_return = std::move(*static_cast<T*>(it->second.first));
			mStorageContent.erase(it);

			mFragHoleSize += sizeof(T);

			return to_return;
		}

		void Remove(const UUID& element)
		{
			const auto it = mStorageContent.find(element);

			if (it->second.first + it->second.second == mStorageFreePtr)
				mStorageFreePtr -= it->second.second;
			else
				mFragHoleSize += it->second.second;
			
			mStorageContent.erase(it);
		}

		// Returns new location of each component
		// Memory has already been moved
		std::unordered_map<UUID, u8*> Defragment()
		{
			std::unordered_map<UUID, u8*> res;

			u8* new_storage = (u8*)calloc(mStorageSize, 1);
			u8* new_storage_ptr = new_storage;

			for (auto& data : mStorageContent)
			{
				memcpy(new_storage_ptr, data.second.first, data.second.second);
				data.second.first = new_storage_ptr;
				new_storage_ptr += data.second.second;

				res.emplace(data.first, data.second.first);
			}

			mFragHoleSize = 0;

			free(mStorage);
			mStorage = new_storage;
			mStorageFreePtr = new_storage_ptr;

			return res;
		}
		std::optional<std::unordered_map<UUID, u8*>> DefragmentIfNeeded()
		{
			if ((float)mFragHoleSize / (float)mStorageSize > mFragThreshold)
			{
				return { Defragment() };
			}
			return {};
		}

		template<typename T> T& Get(const UUID& uuid) { return (T&)*mStorageContent.at(uuid).first; }
		[[nodiscard]] const u8* GetRaw() const { return mStorage; }
		[[nodiscard]] const std::unordered_map<const UUID, std::pair<u8*, usize>>& GetContent() const { return mStorageContent; }
		[[nodiscard]] usize GetSize() const
		{
			usize total_size = 0;
			for(auto& [ptr, size] : mStorageContent | std::ranges::views::values)
			{
				total_size += size;
			}
			return total_size;
		}

		[[nodiscard]] bool IsFullAfter(usize size)
		{
			return (mStorageFreePtr + size) > (mStorage + mStorageSize);
		}

		virtual void ResizeIfNeeded() = 0;

	protected:
		void Resize()
		{
			mStorageSize *= 2;

			u8* new_storage = (u8*)realloc(mStorage, mStorageSize);
			const long long diff = new_storage - mStorage;
			mStorage = new_storage;

			mStorageFreePtr += diff;
			for (auto& data : mStorageContent | std::views::values)
			{
				data.first += diff;
			}

			mStorageContent.reserve(mStorageSize / 2);
		}

		u8* mStorage;
		u8* mStorageFreePtr;
		size_t mStorageSize;

		std::unordered_map<const UUID, std::pair<u8*, usize>> mStorageContent;

		float mFragThreshold;
		size_t mFragHoleSize;
	};

	// ArenaContainer + Component handles
	class ComponentContainer : protected ArenaContainer
	{
	public:
		ComponentContainer(size_t initial_size = 1024, float hole_threshold = 0.10f) : ArenaContainer(initial_size, hole_threshold) {}

		template<typename T> Handle<T> Insert(T&& component)
		{
			const UUID uuid;
			return Handle<T>(InsertExternal(uuid, sizeof(T)), component, uuid);
		}

		template<typename ...T>
		void Insert(T&& ...handles)
		{
			([&](auto&& handle)
				{
					using HandleType = std::decay_t<decltype(handle)>;
					static_assert(std::is_base_of_v<IHandle, HandleType>, "All inputs should be IHandle's");
					mHandles.emplace(handle->Id, std::forward<HandleType>(handle));
					handle->Move(InsertExternal(handle->Id, handle->Size));
				}(std::forward<T>(handles)), ...);
		}

		template<typename T> void Insert(Handle<T>& component_handle)
		{
			Insert((IHandle*)&component_handle);
		}

		void Insert(IHandle* handle)
		{
			CORE_ASSERT(!mHandles.contains(handle->Id), "Component with ID already in container")

			mHandles.emplace(handle->Id, handle);
			handle->Move(InsertExternal(handle->Id, handle->Size));
		}

		void Remove(const UUID& component_uuid)
		{
			DefragmentIfNeeded();
			mHandles.erase(component_uuid);
			ArenaContainer::Remove(component_uuid);
		}
		void Remove(const IHandle* component_handle)
		{
			DefragmentIfNeeded();
			mHandles.erase(component_handle->Id);
			ArenaContainer::Remove(component_handle->Id);
		}
		template<typename T> T&& Remove(UUID& component_uuid)
		{
			mHandles.erase(component_uuid);
			return ArenaContainer::Remove<T>(component_uuid);
		}
		template<typename T> T&& Remove(Handle<T>& component_handle)
		{
			mHandles.erase(component_handle.Id);
			return ArenaContainer::Remove(component_handle.Id);
		}

		[[nodiscard]] bool Contains(const UUID& component_uuid) const { return mHandles.contains(component_uuid); }

		void ResizeIfNeeded() override
		{
			Resize();
			for(auto it = mStorageContent.begin(); it != mStorageContent.end(); ++it)
			{
				mHandles[it->first]->Component = (ComponentType*)it->second.first;
			}
		}
		void Defragment()
		{
			std::unordered_map<UUID, u8*> defrag = ArenaContainer::Defragment();
			for (const auto& [id, loc] : defrag)
			{
				mHandles.at(id)->SetLocation(loc);
			}
		}
		void DefragmentIfNeeded()
		{
			if(mFragHoleSize > mStorageSize * mFragThreshold)
			{
				Defragment();
			}
		}
	private:
		// uuid is member of IHandle
		std::unordered_map<const UUID, IHandle*> mHandles;
	};

	// Sequential storage for sets of types
	template<typename ...Ts>
	class TupleComponentContainer : public ArenaContainer
	{
	public:
		using TypeInTuple = std::tuple<Ts...>;
		size_t TotalSize = 0;

		// initial_size in #Tuples
		TupleComponentContainer(size_t initial_size = 10, float hole_threshold = 0.1f)
		{
			(void(TotalSize += sizeof(Ts)), ...);
			ArenaContainer(initial_size * TotalSize, hole_threshold);
		}

		UUID Insert(Handle<Ts>&... handles)
		{
			UUID id;
			mEntities.emplace(id, std::make_tuple(handles...));
			(void(handles.Move(InsertExternal(handles.Id, handles.Size))), ...);
			return id;
		}

		void Remove(UUID& entity_uuid)
		{
			CORE_ASSERT(mEntities.contains(entity_uuid), "Does not have this entity")

			std::tuple<Handle<Ts>&...>& handles = mEntities[entity_uuid];
			ApplyToTuple(handles, [&](auto handle)
				{
					ArenaContainer::Remove(handle.Id);
				});
			mEntities.erase(entity_uuid);
		}

		void Move(UUID& entity_uuid, u8* location)
		{
			CORE_ASSERT(mEntities.contains(entity_uuid), "Does not have this entity")

			std::tuple<Handle<Ts>&...>& handles = mEntities[entity_uuid];
			ApplyToTuple(handles, [&](auto& handle)
				{
					handle.Move(location);
					ArenaContainer::Remove(handle.Id);
				});
			mEntities.erase(entity_uuid);
		}
		
	private:
		std::unordered_map<UUID, std::tuple<Handle<Ts>&...>> mEntities;
	};

	// Not completely efficient
	// It could be more array focused
	template<typename T>
	class VectorComponentContainer : protected ComponentContainer
	{
	public:
		// initial_size in #elements
		VectorComponentContainer(usize initial_size) : ComponentContainer(initial_size * sizeof(T)) {}

		void Insert(Handle<T>& handle)
		{
			mHandles.emplace(handle.Id, handle);
			handle.Move(InsertExternal(handle.Id, handle.Size));
		}

		T&& Remove(const UUID& element)
		{
						mHandles.erase(element);
			return ArenaContainer::Remove<T>(element);
		}

		T&& Remove(Handle<T>& handle)
		{
						mHandles.erase(handle.Id);
			return ArenaContainer::Remove<T>(handle.Id);
		}

		[[nodiscard]] inline T* GetData() const { return (T*)GetRaw(); }
		 
		// Returns the number of T's in storage
		[[nodiscard]] inline usize GetSize() const { return mStorageContent.size(); }

	private:
		std::unordered_map<const UUID, Handle<T>*> mHandles;
	};


	//			Inefficient because removed value space does not get reused
	// Stores keys in std::unordered_map, stores values on heap
	// References to values are always valid
	template<typename K, typename V>
	class StaticMap
	{
	public:
		struct Iterator		// ITERATOR
		{
			friend class StaticMap;

			[[nodiscard]] V* Get() const
			{
				return &p->second;
			}

			V& operator*()
			{
				CH_PROFILE_FUNCTION();
				return &p->second;
			}
			V* operator->()
			{
				CH_PROFILE_FUNCTION();
				return p->second;
			}

			bool operator != (const Iterator& rhs) const {
				return p != rhs.p;
			}

			Iterator& operator++()
			{
				++p;
				while (p != mPairs.end() && !(
					(!mFilter.has_value() || (*mFilter)(*p)))
					) ++p;
				return *this;
			}

			Iterator& operator++(int)
			{
				Iterator& tmp = *this;
				operator++();
				return tmp;
			}

			Iterator end()
			{
				return Iterator(mPairs.end());
			}

			// private constructor
		private:
			Iterator(std::unordered_map<const K, V*>& pairs, std::optional<std::function<bool(std::pair<const K, V*>&)>&&> filter = nullptr)
				: mPairs(pairs), p(pairs.begin()), mFilter(filter) {}

		private:
			std::unordered_map<const K, V*>::iterator p;
			std::unordered_map<const K, V*>& mPairs;
			std::optional<std::function<bool(std::pair<const K, V*>&)>> mFilter;
		};	// ITERATOR

	public:
		// number_components_local = no. of components on the local storage before needing to allocate on heap
		StaticMap(usize number_components_local = 1024)
			: mLocalStorageSize(number_components_local)
		{
			mLocalStorage = new V[number_components_local];
			mLocalStoragePtr = mLocalStorage;
		}

		// V& insert(K&& key, V&& value)
		// {
		// 	return emplace(key, value);
		// }

		V& emplace(K key, V&& value)
		{
			if ((mLocalStoragePtr - mLocalStorage + sizeof(V)) <= mLocalStorageSize)
			{
				if (mHoles.size() > 0)
				{
					V* loca = mHoles.front();
					mHoles.pop_front();
					mPairs.emplace(key, new(loca) V(value));
					return *loca;
				}

				V& ret = *mLocalStoragePtr;
				mPairs.emplace(key, new(mLocalStoragePtr) V(value));
				mLocalStoragePtr += sizeof(V);
				return ret;
			}
			return *mPairs.emplace(key, new V(value)).first->second;
		}

		V&& remove(K& key)
		{
			V ret = *mPairs[key];
			if (mPairs[key] < mLocalStorage || mPairs[key] >= mLocalStorage + mLocalStorageSize)
			{
				delete mPairs[key];
			} else
			{
				mHoles.push_back(mPairs[key]);
			}
			mPairs.erase(key);
			return std::move(ret);
		}

		bool contains(K& key)
		{
			return mPairs.contains(key);
		}

		u32 size() const
		{
			return mPairs.size();
		}


		V& at(K& key) { return *mPairs[key]; }
		V& operator[](K& key) { return at(key); }



		inline Iterator begin()
		{
			return Iterator(mPairs);
		}
		inline Iterator end()
		{
			return Iterator(mPairs, mPairs.end());
		}
		inline Iterator FilterBy(std::function<bool(std::pair<const K, V*>&)>&& filter_function)
		{
			return Iterator(std::forward<std::function<bool(std::pair<const K, V*>&)>&& >(filter_function));
		}
	private:
		V* mLocalStorage;
		V* mLocalStoragePtr;
		usize mLocalStorageSize;

		std::unordered_map<const K, V*> mPairs;
		std::list<V*> mHoles;
	};
}

#endif
