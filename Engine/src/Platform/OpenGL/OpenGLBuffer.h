#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Graphics/Buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace bron
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(usize size);
		OpenGLVertexBuffer(float* vertices, usize size);

		~OpenGLVertexBuffer() override
		{
		}

		void Bind() override;
		void Unbind() override;

		const BufferLayout& GetBufferLayout() const override;
		void SetBufferLayout(const BufferLayout& layout) override;
		void SetBufferData(const void* data, usize size) override;
		void ResizeAndSetBuffer(const void* data, usize size) override;


	private:
		u32 render_id_;
		BufferLayout layout_;
	};


	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(u32 count);
		OpenGLIndexBuffer(u32* indices, u32 count);

		~OpenGLIndexBuffer() override
		{
		}

		void Bind() override;
		void Unbind() override;

		u32 GetCount() const override;
		void SetBufferData(u32* data, u32 count) override;
		void ResizeAndSetBuffer(u32* data, u32 count) override;

	private:
		u32 render_id_;
		u32 count_;
	};

	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(usize size, u32 binding);
		OpenGLUniformBuffer(const void* data, usize size, u32 binding);
		~OpenGLUniformBuffer() override;

		void Bind(u32 binding) override;
		void Unbind() override;
		bool IsBound() override;

		void SetData(const uint8_t *data, uint64_t size, uint64_t offset = 0) override;
		void ResizeAndSetData(const uint8_t *data, uint64_t size) override;

	private:
		GLuint rendererID;
		usize bufferSize;
		u32 current_binding_;
		bool bound;
	};
}
