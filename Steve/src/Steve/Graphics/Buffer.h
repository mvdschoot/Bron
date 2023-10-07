#ifndef RENDERER_BUFFER_HEADER
#define RENDERER_BUFFER_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "ShaderDataFunctions.h"

#include <string>
#include <vector>
#include <stdlib.h>

#include "glad/glad.h"

namespace Steve
{

	struct STEVE_API BufferElement
	{
		std::string name;
		ShaderDataType type;
		u32 size;
		u32 offset;
		bool normalized;

		BufferElement() = default;

		BufferElement(std::string name, ShaderDataType type) :
			name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(false)
		{
		}

		~BufferElement() = default;

		u32 getComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float: return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Mat3: return 3; // 3* float3
			case ShaderDataType::Mat4: return 4; // 4* float4
			case ShaderDataType::Int: return 1;
			case ShaderDataType::Int2: return 2;
			case ShaderDataType::Int3: return 3;
			case ShaderDataType::Int4: return 4;
			case ShaderDataType::Bool: return 1;
			}

			return 0;
		}
	};

	class STEVE_API BufferLayout
	{
	public:
		BufferLayout(): _stride(0)
		{
		};

		BufferLayout(std::initializer_list<BufferElement> elements) :
			_stride(0), _buffer_elements(elements)
		{
			CalculateInfo();
		}

		//~_BufferLayout() = default;
		
		const std::vector<BufferElement>& GetElements() const { return _buffer_elements; }
		u32 GetStride() const { return _stride; }

		void CalculateInfo()
		{
			u32 offset = 0;
			_stride = 0;
			for (auto& it : _buffer_elements)
			{
				it.offset = offset;
				offset += it.size;
				_stride += it.size;
			}
		}
	protected:
		void SetElements(std::vector<BufferElement>&& buffer)
		{
			_buffer_elements = buffer;
			CalculateInfo();
		}


		u32 _stride;
		std::vector<BufferElement> _buffer_elements;
	};

	class STEVE_API VertexBuffer
	{
	public:
		VertexBuffer()
		{
		};

		virtual ~VertexBuffer()
		{
		};

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual const BufferLayout& getBufferLayout() const = 0;
		virtual void setBufferLayout(const BufferLayout& layout) = 0;
		virtual void setBufferData(const void* data, usize size) = 0;
		virtual void resizeAndSetBuffer(const void* data, usize size) = 0;

		static Ref<VertexBuffer> Create(usize size);
		static Ref<VertexBuffer> Create(float* vertices, usize size);
	};


	class STEVE_API IndexBuffer
	{
	public:
		IndexBuffer()
		{
		};

		virtual ~IndexBuffer()
		{
		};

		virtual void bind() = 0;
		virtual void unbind() = 0;

		[[nodiscard]] virtual u32 getCount() const = 0;
		virtual void setBufferData(u32* data, u32 count) = 0;
		virtual void resizeAndSetBuffer(u32* data, u32 count) = 0;

		static Ref<IndexBuffer> Create(u32* indices, u32 count);
		static Ref<IndexBuffer> Create(usize size);
	};
}


#endif
