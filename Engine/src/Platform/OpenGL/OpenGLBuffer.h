#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Graphics/Buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Bron
{
	class OpenGLVertexBuffer final : public VertexBuffer
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


	class OpenGLIndexBuffer final : public IndexBuffer
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

	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(usize size, u32 binding);
		OpenGLUniformBuffer(const void* data, usize size, u32 binding);
		~OpenGLUniformBuffer() override;

		void bind(u32 binding) override;
		void unbind() override;
		bool isBound() override;

		void setData(const uint8_t *data, uint64_t size, uint64_t offset = 0) override;
		void resizeAndSetData(const uint8_t *data, uint64_t size) override;

	private:
		GLuint rendererID;
		usize bufferSize;
		u32 currentBinding;
		bool bound;
	};
}
