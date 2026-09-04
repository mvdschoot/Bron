#include "Grid.h"

#include "Command.h"
#include "Bron/Graphics/BuiltinShaders.h"

namespace bron {
struct GridData {
	Ref<VertexArray> vao;
	Ref<VertexBuffer> vbo;
	Ref<IndexBuffer> ibo;

	Ref<Shader> shader;
	Camera* camera;
};

static GridData g_data;

void GridRenderer::Init(Camera* camera) {
	BR_PROFILE_FUNCTION();

	g_data.camera = camera;
	g_data.shader = Shader::Create(builtin_shaders::Source(builtin_shaders::Id::kGrid));


	float vertices[6] = {0, 0, 0, 0, 0, 0};
	g_data.vbo = VertexBuffer::Create(vertices, 6 * sizeof(float));
	g_data.vbo->SetBufferLayout({{"a_Position", ShaderDataType::kFloat}});

	u32 indices[6] = {0, 1, 2, 3, 4, 5};
	g_data.ibo = IndexBuffer::Create(indices, 6);

	g_data.vao = VertexArray::Create();
	g_data.vao->Bind();
	g_data.vao->SetIndexBuffer(g_data.ibo);
	g_data.vao->AddVertexBuffer(g_data.vbo);
}

void GridRenderer::Draw() {
	BR_PROFILE_FUNCTION();

	g_data.shader->Bind();
	// g_data.shader->SetUniformMat4("uPosition", line_render_data.camera->GetVPmatrix());
	g_data.shader->SetUniformMat4("uView", g_data.camera->GetViewMatrix());
	g_data.shader->SetUniformMat4("uProjection", g_data.camera->GetProjectionMatrix());

	Command::DrawIndexed(g_data.vao, 6);
}

} // namespace bron
