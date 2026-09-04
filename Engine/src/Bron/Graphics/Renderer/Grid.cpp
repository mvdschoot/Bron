#include "Grid.h"

#include "Command.h"
#include "Bron/Graphics/BuiltinShaders.h"

namespace bron
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
		BR_PROFILE_FUNCTION();

		gData._Camera = camera;
		gData._Shader = Shader::Create(builtin_shaders::Source(builtin_shaders::Id::Grid));


		float vertices[6] = { 0, 0, 0, 0, 0, 0};
		gData.Vbo = VertexBuffer::Create(vertices, 6 * sizeof(float));
		gData.Vbo->SetBufferLayout({
			{"a_Position", ShaderDataType::Float}
			});

		u32 indices[6] = {0,1,2,3,4,5};
		gData.Ibo = IndexBuffer::Create(indices, 6);

		gData.Vao = VertexArray::Create();
		gData.Vao->Bind();
		gData.Vao->SetIndexBuffer(gData.Ibo);
		gData.Vao->AddVertexBuffer(gData.Vbo);
	}

	void GridRenderer::Draw()
	{
		BR_PROFILE_FUNCTION();

		gData._Shader->Bind();
		// gData._Shader->SetUniformMat4("uPosition", lineRenderData.camera->GetVPmatrix());
		gData._Shader->SetUniformMat4("uView", gData._Camera->GetViewMatrix());
		gData._Shader->SetUniformMat4("uProjection", gData._Camera->GetProjectionMatrix());

		Command::DrawIndexed(gData.Vao, 6);
	}
	
}
