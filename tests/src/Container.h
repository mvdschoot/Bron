#ifndef __CUSTOMSTORAGE_HEADER__
#define __CUSTOMSTORAGE_HEADER__

#include <map>
#include <ranges>

#include "UUID.h"

namespace Steve
{

	class DynamicContainer
	{
	public:
		DynamicContainer(size_t initial_size = 1024, float hole_threshold = 0.10) : s_StorageSize(initial_size), s_HoleThreshold(hole_threshold)
		{
			m_Storage = (char*)malloc(s_StorageSize);
			m_StorageFreePtr = m_Storage;
		}

		~DynamicContainer()
		{
			free(m_Storage);
		}

		template<class T>
		T* Insert(UUID uuid, T&& element)
		{
			if (m_StorageFreePtr + sizeof(T) >= m_Storage + s_StorageSize)
				Resize();

			size_t size = sizeof(T);
			T* ptr = new(m_StorageFreePtr) T(element);

			m_StorageContent.emplace(uuid, std::make_pair((char*)ptr, size));
			m_StorageFreePtr += size;

			return ptr;
		}

		template<class T>
		T* Insert(T&& element)
		{
			if (m_StorageFreePtr + sizeof(T) >= m_Storage + s_StorageSize)
				Resize();

			size_t size = sizeof(T);
			T* ptr = new(m_StorageFreePtr) T(element);

			m_StorageContent.emplace(element.p_Id, std::make_pair((char*)ptr, size));
			m_StorageFreePtr += size;

			return ptr;
		}

		template<class T>
		T&& Remove(const UUID element)
		{
			const auto it = m_StorageContent.find(element);
			T&& to_return = std::move(*static_cast<T*>(it->second.first));
			m_StorageContent.erase(it);

			m_HoleSize += sizeof(T);
			if (m_HoleSize > s_StorageSize * s_HoleThreshold)
			{
				Defragment();
			}

			return to_return;
		}

		void Remove(const UUID& element)
		{
			auto it = m_StorageContent.find(element);
			m_HoleSize += it->second.second;
			m_StorageContent.erase(it);

			if (m_HoleSize > static_cast<size_t>(s_StorageSize * s_HoleThreshold))
			{
				Defragment();
			}
		}

	private:
		void Resize()
		{
			s_StorageSize *= 2;

			char* new_storage = (char*)realloc(m_Storage, s_StorageSize);
			const long long diff = new_storage - m_Storage;
			m_Storage = new_storage;

			m_StorageFreePtr += diff;
			for(auto& data : m_StorageContent | std::views::values)
			{
				data.first += diff;
			}
		}

		void Defragment()
		{
			char* new_storage = (char*)malloc(s_StorageSize);
			char* new_storage_ptr = new_storage;

			for (auto& data : m_StorageContent | std::views::values)
			{
				memcpy_s(new_storage_ptr, data.second, data.first, data.second);
				data.first = new_storage_ptr;
				new_storage_ptr += data.second;
			}

			free(m_Storage);
			m_Storage = new_storage;
			m_StorageFreePtr = new_storage_ptr;
		}

		char* m_Storage;
		char* m_StorageFreePtr;
		size_t s_StorageSize;

		std::map<UUID, std::pair<char*, size_t>> m_StorageContent;

		float s_HoleThreshold;
		size_t m_HoleSize = 0;
	};
}

#endif
