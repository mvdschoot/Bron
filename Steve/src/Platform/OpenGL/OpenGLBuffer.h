#ifndef OPENGLBUFFER_HEADER
#define OPENGLBUFFER_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Graphics/Buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Steve::graphics
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(usize size);
		OpenGLVertexBuffer(float* vertices, usize size);

		~OpenGLVertexBuffer() override
		{
		}

		void bind() override;
		void unbind() override;

		const BufferLayout& getBufferLayout() const override;
		void setBufferLayout(const BufferLayout& layout) override;
		void setBufferData(const void* data, usize size) override;
		void resizeAndSetBuffer(const void* data, usize size) override;


	private:
		u32 _render_id;
		BufferLayout _layout;
	};


	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(u32 count);
		OpenGLIndexBuffer(u32* indices, u32 count);

		~OpenGLIndexBuffer() override
		{
		}

		void bind() override;
		void unbind() override;

		u32 getCount() const override;
		void setBufferData(u32* data, u32 count) override;
		void resizeAndSetBuffer(u32* data, u32 count) override;

	private:
		u32 _render_id;
		u32 _count;
	};
}


#endif
