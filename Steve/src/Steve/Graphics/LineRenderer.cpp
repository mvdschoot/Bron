#include "LineRenderer.h"

#include "Renderer/Command.h"

namespace Steve::graphics
{
	struct LineVertex
	{
		glm::vec2 pos;
		glm::vec4 color;
		float width;
	};

	struct LineRendererData
	{
		static constexpr u32 maxLines = 1e4;
		static constexpr u32 maxLineStripIndices = 1e4;

		// Separate lines
		Ref<VertexArray> lineVertexArray;
		Ref<VertexBuffer> lineBuffer;
		Ref<Shader> lineShader;

		LineVertex* lineVertexBufferBase;
		LineVertex* lineVertexBufferPtr;
		u32 lineIndexCount = 0;
		u32 lineCount = 0;

		// Line strips
		Ref<VertexArray> lineStripVertexArray;
		Ref<VertexBuffer> lineStripBuffer;

		LineVertex* lineStripVertexBufferBase;
		LineVertex* lineStripVertexBufferPtr;
		u32 lineStripIndexCount = 0;
		u32 lineStripCount = 0;

		Camera* camera;
	};

	static LineRendererData s_linedata;

	void LineRenderer::Init()
	{
		CH_PROFILE_FUNCTION();
		// Line setup
		{
			s_linedata.lineVertexArray = VertexArray::Create();
			s_linedata.lineBuffer = VertexBuffer::Create(LineRendererData::maxLines * 2 * sizeof(LineVertex));

			const BufferLayout line_layout({
				{"a_Position", ShaderDataType::Float2},
				{"a_Color", ShaderDataType::Float4},
				{"a_Width", ShaderDataType::Float}
				});
			s_linedata.lineBuffer->setBufferLayout(line_layout);
			s_linedata.lineVertexArray->addVertexBuffer(s_linedata.lineBuffer);

			s_linedata.lineVertexBufferBase = new LineVertex[LineRendererData::maxLines * 2];
			s_linedata.lineVertexBufferPtr = s_linedata.lineVertexBufferBase;
			memset(s_linedata.lineVertexBufferBase, 0, LineRendererData::maxLines * 2);

			const auto lineIndexBuffer = new u32[LineRendererData::maxLines * 2];
			int offset = 0;
			for (u32 x = 0; x < LineRendererData::maxLines * 2; x += 2)
			{
				lineIndexBuffer[x + 0] = x + 0;
				lineIndexBuffer[x + 1] = x + 1;
			}
			const Ref<IndexBuffer> IBuffer = IndexBuffer::Create(lineIndexBuffer, LineRendererData::maxLines * 2);
			s_linedata.lineVertexArray->setIndexBuffer(IBuffer);
			delete[] lineIndexBuffer;
		}

		// Line Strip setup
		{
			s_linedata.lineStripVertexArray = VertexArray::Create();
			s_linedata.lineStripBuffer = VertexBuffer::Create(LineRendererData::maxLineStripIndices * sizeof(LineVertex));

			const BufferLayout line_layout({
				{"a_Position", ShaderDataType::Float2},
				{"a_Color", ShaderDataType::Float4},
				{"a_Width", ShaderDataType::Float}
			});
			s_linedata.lineStripBuffer->setBufferLayout(line_layout);
			s_linedata.lineStripVertexArray->addVertexBuffer(s_linedata.lineStripBuffer);

			s_linedata.lineStripVertexBufferBase = new LineVertex[LineRendererData::maxLineStripIndices];
			s_linedata.lineStripVertexBufferPtr = s_linedata.lineStripVertexBufferBase;
			memset(s_linedata.lineStripVertexBufferBase, 0, LineRendererData::maxLineStripIndices);

			const auto lineStripIndexBuffer = new u32[LineRendererData::maxLineStripIndices];
			int offset = 0;
			u32 x;
			for (x = 0; x < LineRendererData::maxLineStripIndices; x += 2)
			{
				lineStripIndexBuffer[x + 0] = x + 0;
				lineStripIndexBuffer[x + 1] = x + 1;
			}
			const Ref<IndexBuffer> IBuffer = IndexBuffer::Create(lineStripIndexBuffer, LineRendererData::maxLineStripIndices);
			s_linedata.lineStripVertexArray->setIndexBuffer(IBuffer);
			delete[] lineStripIndexBuffer;
		}

		s_linedata.lineShader = Shader::CreateShaderFromLocation("../../../Steve/Assets/line_shader.glsl");
		s_linedata.lineShader->bind();
	}

	void LineRenderer::Start(Camera* camera)
	{
		s_linedata.camera = camera;

		s_linedata.lineVertexBufferPtr = s_linedata.lineVertexBufferBase;
		s_linedata.lineIndexCount = 0;
		s_linedata.lineCount = 0;

		s_linedata.lineStripVertexBufferPtr = s_linedata.lineStripVertexBufferBase;
		s_linedata.lineStripIndexCount = 0;
		s_linedata.lineStripCount = 0;
	}

	void LineRenderer::DrawLine(glm::vec3 a, glm::vec3 b, glm::vec4 color, float width)
	{
		CH_PROFILE_FUNCTION();

		if (s_linedata.lineCount == LineRendererData::maxLines)
		{
			End();
			Start(s_linedata.camera);
		}

		s_linedata.lineVertexBufferPtr->pos = a;
		s_linedata.lineVertexBufferPtr->color = color;
		s_linedata.lineVertexBufferPtr->width = width;
		s_linedata.lineVertexBufferPtr++;

		s_linedata.lineVertexBufferPtr->pos = b;
		s_linedata.lineVertexBufferPtr->color = color;
		s_linedata.lineVertexBufferPtr->width = width;
		s_linedata.lineVertexBufferPtr++;

		s_linedata.lineIndexCount += 2;
		s_linedata.lineCount++;
	}

	void LineRenderer::DrawLineStrip(std::vector<glm::vec3>& vertices, glm::vec4 color, float width)
	{
		CH_PROFILE_FUNCTION();

		int size = vertices.size();
		if (s_linedata.lineStripIndexCount + size > LineRendererData::maxLineStripIndices)
		{
			End();
			Start(s_linedata.camera);
		}

		for(int x = 0; x < size; x++)
		{
			s_linedata.lineStripVertexBufferPtr->pos = vertices[x];
			s_linedata.lineStripVertexBufferPtr->color = color;
			s_linedata.lineStripVertexBufferPtr->width = width;
			s_linedata.lineStripVertexBufferPtr++;

			s_linedata.lineStripIndexCount += 1;
		}

		s_linedata.lineStripCount++;
	}

	void LineRenderer::End()
	{
		flush();
	}

	// Get info
	u32 LineRenderer::GetLineCount()
	{
		return s_linedata.lineCount;
	}
	u32 LineRenderer::GetLineIndexCount()
	{
		return s_linedata.lineIndexCount;
	}
	u32 LineRenderer::GetLineStripCount()
	{
		return s_linedata.lineStripCount;
	}
	u32 LineRenderer::GetLineStripIndexCount()
	{
		return s_linedata.lineStripIndexCount;
	}

	void LineRenderer::flush()
	{
		CH_PROFILE_FUNCTION();

		s_linedata.lineShader->bind();
		s_linedata.lineShader->setUniformMat4("uVPmatrix", s_linedata.camera->GetVPmatrix());

		// Line render
		if (s_linedata.lineIndexCount != 0) {
			s_linedata.lineVertexArray->bind();

			u32 size = static_cast<u32>((uint8_t*)s_linedata.lineVertexBufferPtr - (uint8_t*)s_linedata.lineVertexBufferBase);
			s_linedata.lineBuffer->setBufferData(s_linedata.lineVertexBufferBase, size);

			Command::DrawIndexedLines(s_linedata.lineVertexArray, s_linedata.lineIndexCount);
		}

		// Line Strip render 
		if (s_linedata.lineStripIndexCount != 0) {
			s_linedata.lineStripVertexArray->bind(); 

			u32 size = static_cast<u32>((uint8_t*)s_linedata.lineStripVertexBufferPtr - (uint8_t*)s_linedata.lineStripVertexBufferBase);
			s_linedata.lineStripBuffer->setBufferData(s_linedata.lineStripVertexBufferBase, size);

			Command::DrawIndexedStripLines(s_linedata.lineStripVertexArray, s_linedata.lineStripIndexCount);
		}
	}
}
