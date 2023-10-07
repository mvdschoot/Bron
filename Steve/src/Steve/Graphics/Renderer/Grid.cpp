#include "Grid.h"

#include "Command.h"

namespace Steve
{
	struct GridData
	{
		Ref<VertexArray> Vao;
		Ref<VertexBuffer> Vbo;
		Ref<IndexBuffer> Ibo;

		Ref<Shader> _Shader;
		Camera* _Camera;
	};

	static GridData gData;

	void GridRenderer::Init(Camera* camera)
	{
		CH_PROFILE_FUNCTION();

		gData._Camera = camera;
		gData._Shader = Shader::CreateShaderFromLocation("../../../Steve/Assets/GridShader.glsl");


		float vertices[6] = { 0, 0, 0, 0, 0, 0};
		gData.Vbo = VertexBuffer::Create(vertices, 6 * sizeof(float));
		gData.Vbo->setBufferLayout({
			{"a_Position", ShaderDataType::Float}
			});

		u32 indices[6] = {0,1,2,3,4,5};
		gData.Ibo = IndexBuffer::Create(indices, 6);

		gData.Vao = VertexArray::Create();
		gData.Vao->bind();
		gData.Vao->setIndexBuffer(gData.Ibo);
		gData.Vao->addVertexBuffer(gData.Vbo);
	}

	void GridRenderer::Draw()
	{
		CH_PROFILE_FUNCTION();

		gData._Shader->bind();
		// gData._Shader->setUniformMat4("uPosition", lineRenderData.camera->GetVPmatrix());
		gData._Shader->setUniformMat4("uView", gData._Camera->GetViewMatrix());
		gData._Shader->setUniformMat4("uProjection", gData._Camera->GetProjectionMatrix());

		Command::DrawIndexed(gData.Vao, 6);
	}
	
}
