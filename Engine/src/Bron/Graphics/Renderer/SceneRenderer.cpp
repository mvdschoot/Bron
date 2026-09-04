#include "SceneRenderer.h"

#include "Command.h"
#include "Bron/Graphics/Phong/PhongDefinitions.h"
#include "Bron/Graphics/ShaderRegistry.h"

#include <map>
#include <string>


namespace bron {
RenderStatistics SceneRenderer::Statistics = {};

struct SceneRendererData {
	Ref<Texture> white_texture;

	static constexpr u32 kTextureSlots = 32;
	u32 texture_array[kTextureSlots]{};
};
static SceneRendererData s_data;

void SceneRenderer::Init() {
	s_data.white_texture = Texture2D::Create(1, 1);
	u32 white_data = 0xffffffff;
	s_data.white_texture->SetData(&white_data, sizeof(u32));

	for (int i = 0; i < bron::SceneRendererData::kTextureSlots; i++) {
		s_data.texture_array[i] = i;
	}
}

void SceneRenderer::Draw(Scene& scene) {
	BR_PROFILE_FUNCTION();

	Statistics = {0, 0, 0, 0, 0};

	BR_CORE_ASSERT(scene.camera != nullptr, "This scene has no camera attached");

	// Sort the drawable entities by shader, then by material, so each is only bound once.
	// Rebuilt every frame: the registry is the single source of truth, and a scene of this size
	// makes the sort free compared to keeping a second container in sync.
	std::map<std::string, std::map<MaterialBase*, std::vector<entt::entity>>> queue;
	for (auto [entity, mesh]: scene.reg.view<MeshComponent>().each()) {
		BR_CORE_ASSERT(mesh.material != nullptr, "Mesh has no material and cannot be drawn");
		queue[mesh.material->shader_name][mesh.material.get()].push_back(entity);
	}

	// Light data is shared by every shader, so upload and bind it once for the whole frame.
	scene.light_management.Bind();

	for (auto& [shader_name, materials]: queue) {
		Statistics.Shaders++;
		Ref<Shader> shader = ShaderRegistry::GetShader(shader_name.c_str());
		shader->Bind();

		shader->SetUniform1iv("u_Textures", (i32*) s_data.texture_array, s_data.kTextureSlots);
		shader->SetUniformMat4("u_Projection", scene.camera->GetProjectionMatrix());
		shader->SetUniformMat4("u_View", scene.camera->GetViewMatrix());
		shader->SetUniform3f("u_ViewPos", scene.camera->GetPosition().x, scene.camera->GetPosition().y,
							 scene.camera->GetPosition().z);
		Statistics.UniformCalls += 3;

		shader->SetUniform1i("u_NumPointLights", scene.light_management.NumberPointLights());

		for (auto& [material, entities]: materials) {
			Statistics.Materials++;

			s_data.white_texture->Bind(0);

			material->Bind(shader, 1);
			Statistics.UniformCalls += material->NumberUniformCalls();

			for (const entt::entity entity: entities) {
				MeshComponent& mesh = scene.reg.get<MeshComponent>(entity);

				shader->SetUniformMat4("u_Model", scene.WorldTransform(entity));
				Statistics.UniformCalls++;

				const Ref<VertexArray> vao = GetVao(mesh, PhongVertexLayout);
				Command::DrawIndexed(vao, vao->GetIndexBuffer()->GetCount());

				Statistics.DrawCalls++;
				Statistics.Meshes++;
			}
		}
	}
}
} // namespace bron
