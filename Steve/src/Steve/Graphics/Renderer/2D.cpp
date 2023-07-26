#include "2D.h"


namespace Steve::graphics
{
	struct QuadVertex
	{
		glm::vec2 pos;
		glm::vec4 color;
		glm::vec2 texCoord;
		glm::vec1 texIndex;
	};

	// Because of multiple shaders
	struct VAO
	{
		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadBuffer;
		Ref<Shader> shader;

		QuadVertex* quadVertexBufferBase;
		QuadVertex* quadVertexBufferPtr;

		u32 quadCount = 0;
	};

	struct Renderer2DData
	{
		static constexpr u32 maxVertices = 1e6;
		static constexpr u32 maxIndices = 1e6;
		static constexpr u32 maxTexUnits = 32;

		u32 quadIndexCount = 0;
		u32 quadVertexCount = 0;

		// Texture stuff
		Ref<Texture> whiteTexture;
		std::array<Ref<Texture>, maxTexUnits> textureSlots;
		u32 currentTexSlot = 1;

		Camera* camera;

		// Everything to graphics
		// toRender[0] == standard rendering
		std::vector<VAO> toRender;
		unsigned int renderState = 0;
	};

	static Renderer2DData sData2D;
#define CurrentRenderData sData2D.toRender[sData2D.renderState]

	void R2D::Init()
	{
		CH_PROFILE_FUNCTION();

		AddVAO(
			Shader::CreateShaderFromLocation("../../../Steve/Assets/2d_shader.glsl"), 
			BufferLayout({
				{"a_Position", ShaderDataType::Float2},
				{"a_Color", ShaderDataType::Float4},
				{"a_TexCoord", ShaderDataType::Float2},
				{"a_TexIndex", ShaderDataType::Float}
			})
		);

		sData2D.whiteTexture = Texture2D::Create(1, 1);
		u32 white_data = 0xffffffff;
		sData2D.whiteTexture->setData(&white_data, sizeof(u32));
	}

	void R2D::BeginScene(Camera* camera)
	{
		CH_PROFILE_FUNCTION();
		sData2D.camera = camera;
		sData2D.quadIndexCount = 0;
		sData2D.quadVertexCount = 0;

		for (VAO &vao : sData2D.toRender)
			vao.quadCount = 0;

		NewBatch();
	}

	void R2D::EndScene()
	{
		CH_PROFILE_FUNCTION();
		Flush();
	}

	void R2D::NewBatch()
	{
		CH_PROFILE_FUNCTION();
		sData2D.currentTexSlot = 1;
		for (VAO& vao : sData2D.toRender) {
			vao.quadVertexBufferPtr = vao.quadVertexBufferBase;
			vao.quadCount = 0;
		}
	}

	void R2D::NextBatch()
	{
		CH_PROFILE_FUNCTION();
		Flush();
		NewBatch();
	}

	uint8_t R2D::AddShader(Ref<Shader> shader, BufferLayout buffer_layout)
	{
		AddVAO(shader, buffer_layout);
		return sData2D.toRender.size() - 1;
	}

	void R2D::ActiveShader(uint8_t shader_number)
	{
		CORE_ASSERT(shader_number < (sData2D.toRender.size()), "Invalid shader number");
		sData2D.renderState = shader_number;
		NextBatch();
	}

	u32 R2D::GetActiveShader()
	{
		return sData2D.renderState;
	}

	void R2D::DrawQuad(const glm::vec3 pos, const glm::vec3 dimension, const glm::vec4 color)
	{
		CH_PROFILE_FUNCTION();

		if (++CurrentRenderData.quadCount * 4 >= Renderer2DData::maxVertices)
		{
			NextBatch();
		}

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = color;
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(0.0f, 0.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(0.0f);
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = color;
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(1.0f, 0.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(0.0f);
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = color;
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(1.0f, 1.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(0.0f);
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = color;
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(0.0f, 1.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(0.0f);
		CurrentRenderData.quadVertexBufferPtr++;

		sData2D.quadVertexCount += 4;
		sData2D.quadIndexCount += 6;
	}

	void R2D::DrawQuad(const glm::vec3 pos, const glm::vec3 dimension, const Ref<Texture> texture)
	{
		CH_PROFILE_FUNCTION();
		if (++CurrentRenderData.quadCount * 4 >= Renderer2DData::maxVertices)
		{
			NextBatch();
		}

		u32 tex_index = 0;
		for (int x = 1; x < sData2D.currentTexSlot; x++)
		{
			if (sData2D.textureSlots[x] != nullptr && (*texture.get()) == (*(sData2D.textureSlots[x])))
			{
				tex_index = x;
				break;
			}
		}
		if (tex_index == 0)
		{
			sData2D.textureSlots[sData2D.currentTexSlot] = texture;
			tex_index = sData2D.currentTexSlot;
			sData2D.currentTexSlot++;
		}

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(0.0f, 0.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(1.0f, 0.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(1.0f, 1.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(0.0f, 1.0f);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		sData2D.quadVertexCount += 4;
		sData2D.quadIndexCount += 6;
	}

	void R2D::DrawQuad(glm::vec3 pos, glm::vec3 dimension, const Ref<Texture> texture,
		glm::vec4 texCoordsAndDims)
	{
		CH_PROFILE_FUNCTION();
		if (++CurrentRenderData.quadCount * 4 >= Renderer2DData::maxVertices)
		{
			NextBatch();
		}
		
		u32 tex_index = 0;
		for (int x = 1; x < sData2D.currentTexSlot; x++)
		{
			if (sData2D.textureSlots[x] != nullptr && (*texture.get()) == (*(sData2D.textureSlots[x])))
			{
				tex_index = x;
				break;
			}
		}

		if (tex_index == 0)
		{
			sData2D.textureSlots[sData2D.currentTexSlot] = texture;
			tex_index = sData2D.currentTexSlot;
			sData2D.currentTexSlot++;
		}

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(texCoordsAndDims.x, texCoordsAndDims.y);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(texCoordsAndDims.x + texCoordsAndDims.z, texCoordsAndDims.y);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(texCoordsAndDims.x + texCoordsAndDims.z, texCoordsAndDims.y + texCoordsAndDims.w);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		CurrentRenderData.quadVertexBufferPtr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CurrentRenderData.quadVertexBufferPtr->color = glm::vec4(1.0f);
		CurrentRenderData.quadVertexBufferPtr->texCoord = glm::vec2(texCoordsAndDims.x, texCoordsAndDims.y + texCoordsAndDims.w);
		CurrentRenderData.quadVertexBufferPtr->texIndex = glm::vec1(static_cast<float>(tex_index));
		CurrentRenderData.quadVertexBufferPtr++;

		sData2D.quadVertexCount += 4;
		sData2D.quadIndexCount += 6;
	}


	u32 R2D::GetTotQuadCount()
	{
		u32 res = 0;
		for (VAO& vao : sData2D.toRender)
			res += vao.quadCount;
		return res;
	}
	u32 R2D::GetTotQuadIndexCount()
	{
		return sData2D.quadIndexCount;
	}

	u32 R2D::GetTotVertexCount()
	{
		return sData2D.quadVertexCount;
	}

	void R2D::AddVAO(Ref<Shader> shader, BufferLayout buffer_layout)
	{
		CH_PROFILE_FUNCTION();
		VAO& vao = sData2D.toRender.emplace_back();

		vao.quadVertexArray = VertexArray::Create();
		vao.quadBuffer = VertexBuffer::Create(sData2D.maxVertices * sizeof(QuadVertex));
		vao.quadBuffer->setBufferLayout(buffer_layout);
		vao.quadVertexArray->addVertexBuffer(vao.quadBuffer);


		vao.quadVertexBufferBase = new QuadVertex[Renderer2DData::maxVertices];
		vao.quadVertexBufferPtr = vao.quadVertexBufferBase;
		memset(vao.quadVertexBufferBase, 0, Renderer2DData::maxVertices);

		const auto quadIndexBuffer = new u32[Renderer2DData::maxIndices];
		int offset = 0;
		for (u32 x = 6; x < Renderer2DData::maxIndices; x += 6)
		{
			quadIndexBuffer[x - 6] = offset + 0;
			quadIndexBuffer[x - 5] = offset + 1;
			quadIndexBuffer[x - 4] = offset + 2;

			quadIndexBuffer[x - 3] = offset + 2;
			quadIndexBuffer[x - 2] = offset + 3;
			quadIndexBuffer[x - 1] = offset + 0;
			offset += 4;
		}
		const Ref<IndexBuffer> IBuffer = IndexBuffer::Create(quadIndexBuffer, Renderer2DData::maxIndices);
		vao.quadVertexArray->setIndexBuffer(IBuffer);
		delete[] quadIndexBuffer;

		vao.shader = shader;

		int arr[Renderer2DData::maxTexUnits];
		for (int x = 0; x < Renderer2DData::maxTexUnits; x++)
		{
			arr[x] = x;
		}

		vao.shader->bind();
		vao.shader->setUniform1iv("uTextures", arr, Renderer2DData::maxTexUnits);

		sData2D.textureSlots[0] = sData2D.whiteTexture;
	}

	void R2D::Flush()
	{
		CH_PROFILE_FUNCTION();

		for (auto& [quadVertexArray, quadBuffer, shader, quadVertexBufferBase, quadVertexBufferPtr, quadCount] : sData2D.toRender) {

			// Quad graphics
			if (quadCount == 0)
				continue;
			quadVertexArray->bind();
			shader->bind();
			for (int x = 0; x < sData2D.currentTexSlot; x++)
			{
				sData2D.textureSlots[x]->bind(x);
			}

			const u32 size = static_cast<u32>((uint8_t*)quadVertexBufferPtr - (uint8_t*)quadVertexBufferBase);
			quadBuffer->setBufferData(quadVertexBufferBase, size);
			shader->setUniformMat4("uVPmatrix", sData2D.camera->GetVPmatrix());
			Command::DrawIndexed(quadVertexArray, quadCount * 6);
		}
	}
}
