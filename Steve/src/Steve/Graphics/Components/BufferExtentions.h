#ifndef _UNIFORMS_HEADRE__
#define _UNIFORMS_HEADRE__

#include "Steve/Graphics/Buffer.h"

namespace Steve
{
	template<typename T>
	struct NamedBufferLayout : public BufferLayout
	{
		NamedBufferLayout(std::initializer_list<std::pair<T, BufferElement>> data)
			: Data({})
		{
			std::vector<BufferElement> b;
			for (std::pair<T, BufferElement> el : data) {
				Data.push_back(el.first);
				b.push_back(el.second);
			}

			SetElements(std::move(b));
		}

		void SetName(T data_type, std::string new_name)
		{
			for (int x = 0; x < Data.size(); x++)
			{
				if (Data[x] == data_type)
				{
					_buffer_elements[x].name = new_name;
				}
			}
		}

		[[nodiscard]] const BufferElement& GetElementData(T light_data_type) const
		{
			for (int x = 0; x < Data.size(); x++)
			{
				if (Data[x] == light_data_type)
				{
					return GetElements()[x];
				}
			}
			CORE_ASSERT(false, "Cannot find the uniform data type type you are looking for.")
		}

		std::vector<T> Data;
	};

	template<typename T>
	struct NamedBufferData
	{
		NamedBufferData(const NamedBufferLayout<T>* layout) : Layout(layout), Data(new u8[layout->GetStride()]())
		{}

		void Set(T type, uint8_t* value)
		{
			const BufferElement& el = Layout->GetElementData(type);
			memcpy(Data + el.offset, value, el.size);
		}

		template<typename S> void Set(T type, S value)
		{
			const BufferElement& el = Layout->GetElementData(type);
			// The element size decides how much is copied, so a value of a different size would either
			// read past the end of 'value' or only partially fill the element.
			CORE_ASSERT(sizeof(S) == el.size, "Value of {} bytes does not match the {} byte element it is written to.",
						sizeof(S), el.size)
			memcpy(Data + el.offset, &value, el.size);
		}

		u8* Get(T type) const
		{
			const BufferElement& el = Layout->GetElementData(type);
			return Data + el.offset;
		}

		template<typename S> S* Get(T type) const
		{
			const BufferElement& el = Layout->GetElementData(type);
			return (S*)(Data + el.offset);
		}

		const NamedBufferLayout<T>* Layout;
		u8* Data;
	};

	template<typename T>
	struct MultipleNamedBufferData
	{
		MultipleNamedBufferData(const NamedBufferLayout<T>* layout, const u8 numberOfBuffers)
				: Layout(layout), NumberOfBuffers(numberOfBuffers), Data(new u8[layout->GetStride() * numberOfBuffers])
		{}

		/**
		 * Sets sequential data to a new buffer according to the buffer layout.
		 *
		 * E.g.: Say we want to fill up 80 buffers, and place the normal coordinates in every buffer. The buffer layout
		 * tells us that the normal coordinate is at offset 12 within each buffer, so we take the sequential normal
		 * coordinates from the 'values' input, split it, and place it at offset within each buffer.
		 */
		void Set(T type, u8* values)
		{
			const BufferElement& el = Layout->GetElementData(type);
			for (int bufferOffset = 0; bufferOffset < NumberOfBuffers; bufferOffset++) {
				memcpy(Data + (bufferOffset * Layout->GetStride()) + el.offset, values + bufferOffset * el.size, el.size);
			}
		}

		u32 GetTotalSize() {
			return NumberOfBuffers * Layout->GetStride();
		}

		const NamedBufferLayout<T>* Layout;
		const u8 NumberOfBuffers;
		u8* Data;
	};
}

#endif