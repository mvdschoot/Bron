#ifndef __HANDLE_HEADER__ 
#define __HANDLE_HEADER__ 

#include <set>

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include <typeindex>

#include "Steve/Core/UUID.h"

namespace Steve
{
	struct ComponentType {};

	class IHandle {
		friend class Entity;
	public:
		// Copy constructor
		IHandle(const IHandle& handle) = default;

		// Move constructor
		IHandle(IHandle&& comp) = default;

		// Constructors
		IHandle() : Type(std::type_index(typeid(ComponentType))), Size(0) {}
		IHandle(std::type_index type, usize size, const UUID& id = UUID()) : Type(type), Id(id), Size(size) {}

		// Destructor IMPORTANT!
		virtual ~IHandle() = default;

		// Less operator for indexing in hash table
		friend bool operator<(const IHandle& l, const IHandle& r)
		{
			return l.Id < r.Id;
		}

		[[nodiscard]] virtual ComponentType* GetComponent() { return Component; }

		void Move(u8* location)
		{
			if (Component != nullptr)
			{
				memcpy(location, Component, Size);
				Component = (ComponentType*)location;
			}
		}
		void SetLocation(u8* new_location)
		{
			Component = (ComponentType*)new_location;
		}

	public:
		const std::type_index Type;
		const UUID Id;
		ComponentType* Component = nullptr;
		const usize Size;
	};

	template<typename T>
	class Handle : public IHandle
	{
		// using T = std::remove_reference_t<Twrong>;
	public:
		operator T&() const {
			return *(T*)Component;
		}

		T* operator->()
		{
			CORE_ASSERT(Component, "Handle is empty")
				return static_cast<T*>(static_cast<void*>(Component));
		}

		T& operator*()
		{
			CORE_ASSERT(Component, "Handle is empty")
				return *static_cast<T*>(static_cast<void*>(Component));
		}

		const T* operator->() const
		{
			CORE_ASSERT(Component, "Handle is empty")
				return static_cast<const T*>(static_cast<const void*>(Component));
		}

		const T& operator*() const
		{
			CORE_ASSERT(Component, "Handle is empty")
				return *static_cast<const T*>(static_cast<const void*>(Component));
		}

		Handle<T>& operator=(const Handle<T>& o)
		{
			Component = o.Component;
			return *this;
		}

		bool operator==(const Handle& other) const
		{
			CORE_ASSERT(Component && other.Component, "Handle is empty")
			return Component == other.Component;
		}

		~Handle() override
		{
		}

		// Copy constructor
		Handle(const Handle& handle) : IHandle(handle) {}

		// Move constructor
		Handle(Handle&& handle) noexcept : IHandle(std::move(handle)) {}

		// Initializes base class
		Handle(const UUID id = UUID()) : IHandle(std::type_index(typeid(T)), sizeof(T), id) {}

		Handle(T& e) : Handle()
        {
            Component = (ComponentType*) & e;
        }

		Handle(T* e) : Handle()
		{
			Component = (ComponentType*)e;
		}

		Handle(void* location, T&& e, const UUID id = UUID()) : Handle(id)
		{
			Component = new(location) T(e);
		}


		const T* Get() const 
		{
			CORE_ASSERT(Component, "Handle is empty")
			return Component;
		}
	};

	template<typename I>
	struct RemoveHandle
	{
		using type = I;
	};

	template<typename I>
	struct RemoveHandle<Handle<I>>
	{
		using type = typename RemoveHandle<I>::type;
	};
}


#endif
