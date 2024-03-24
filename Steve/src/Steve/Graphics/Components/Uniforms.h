#ifndef _UNIFORMS_HEADRE__
#define _UNIFORMS_HEADRE__

namespace Steve
{
	template<typename T>
	struct UniformLayout : public BufferLayout
	{
		UniformLayout(std::initializer_list<std::pair<T, BufferElement>> data)
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
	struct UniformData
	{
		UniformData(const UniformLayout<T>* layout) : Layout(layout), Data(new u8[layout->GetStride()])
		{}

		void Set(T type, const uint8_t* value)
		{
			const BufferElement& el = Layout->GetElementData(type);
			memcpy(Data + el.offset, value, el.size);
		}

		template<typename S> void Set(T type, S value)
		{
			const BufferElement& el = Layout->GetElementData(type);
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

		const UniformLayout<T>* Layout;
		u8* Data;
	};
}

#endif