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

	static GridData g_data;

	void GridRenderer::Init(Camera* camera)
	{
		BR_PROFILE_FUNCTION();

		g_data._Camera = camera;
		g_data._Shader = Shader::Create(builtin_shaders::Source(builtin_shaders::Id::kGrid));


		float vertices[6] = { 0, 0, 0, 0, 0, 0};
		g_data.Vbo = VertexBuffer::Create(vertices, 6 * sizeof(float));
		g_data.Vbo->SetBufferLayout({
			{"a_Position", ShaderDataType::kFloat}
			});

		u32 indices[6] = {0,1,2,3,4,5};
		g_data.Ibo = IndexBuffer::Create(indices, 6);

		g_data.Vao = VertexArray::Create();
		g_data.Vao->Bind();
		g_data.Vao->SetIndexBuffer(g_data.Ibo);
		g_data.Vao->AddVertexBuffer(g_data.Vbo);
	}

	void GridRenderer::Draw()
	{
		BR_PROFILE_FUNCTION();

		g_data._Shader->Bind();
		// g_data._Shader->SetUniformMat4("uPosition", line_render_data.camera->GetVPmatrix());
		g_data._Shader->SetUniformMat4("uView", g_data._Camera->GetViewMatrix());
		g_data._Shader->SetUniformMat4("uProjection", g_data._Camera->GetProjectionMatrix());

		Command::DrawIndexed(g_data.Vao, 6);
	}
	
}
