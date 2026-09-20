#include "SceneRenderer.h"

#include "Command.h"
#include "Bron/Graphics/Texture.h"
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

namespace {
// Entities grouped by shader, then by material, so each is only bound once.
using RenderQueue = std::map<std::string, std::map<MaterialBase*, std::vector<entt::entity>>>;

// Rebuilt every frame: the registry is the single source of truth, and a scene of this size
// makes the sort free compared to keeping a second container in sync.
void Enqueue(Scene& scene, const entt::entity entity, RenderQueue& queue) {
	if (!scene.IsVisible(entity))
		return;

	const MeshComponent& mesh = scene.reg.get<MeshComponent>(entity);
	BR_CORE_ASSERT(mesh.material != nullptr, "Mesh has no material and cannot be drawn");

	queue[mesh.material->shader_name][mesh.material.get()].push_back(entity);
}

// Walks the queue and draws it with each entity's own shader and material. Shared by the
// main pass and by the outline's stencil pass, which needs the same geometry under the
// same shader - only the write masks around it differ.
void Submit(Scene& scene, const CameraView& view, const RenderQueue& queue) {
	for (const auto& [shader_name, materials]: queue) {
		SceneRenderer::Statistics.Shaders++;
		const Ref<Shader> shader = ShaderRegistry::GetShader(shader_name.c_str());
		shader->Bind();

		shader->SetUniform1iv("u_Textures", (i32*) s_data.texture_array, s_data.kTextureSlots);
		shader->SetUniformMat4("u_Projection", view.projection);
		shader->SetUniformMat4("u_View", view.view);
		shader->SetUniform3f("u_ViewPos", view.position.x, view.position.y, view.position.z);
		SceneRenderer::Statistics.UniformCalls += 4;

		shader->SetUniform1i("u_NumPointLights", scene.light_management.NumberPointLights());

		for (const auto& [material, entities]: materials) {
			SceneRenderer::Statistics.Materials++;

			s_data.white_texture->Bind(0);

			material->Bind(shader, 1);
			SceneRenderer::Statistics.UniformCalls += material->NumberUniformCalls();

			for (const entt::entity entity: entities) {
				MeshComponent& mesh = scene.reg.get<MeshComponent>(entity);

				shader->SetUniformMat4("u_Model", scene.WorldTransform(entity));
				SceneRenderer::Statistics.UniformCalls++;

				int entity_id = static_cast<std::uint32_t>(entity);
				shader->SetUniform1iv("u_EntityId", &entity_id, 1);
				SceneRenderer::Statistics.UniformCalls++;

				const Ref<VertexArray> vao = GetVao(mesh, kPhongVertexLayout);
				Command::DrawIndexed(vao, vao->GetIndexBuffer()->GetCount());

				SceneRenderer::Statistics.DrawCalls++;
				SceneRenderer::Statistics.Meshes++;
			}
		}
	}
}
} // namespace

void SceneRenderer::Init() {
	s_data.white_texture = Texture2D::Create(1, 1);
	u32 white_data = 0xffffffff;
	s_data.white_texture->SetData(&white_data, sizeof(u32));

	for (int i = 0; i < bron::SceneRendererData::kTextureSlots; i++) {
		s_data.texture_array[i] = i;
	}
}

void SceneRenderer::Draw(Scene& scene, const CameraView& view) {
	BR_PROFILE_FUNCTION();

	Statistics = {0, 0, 0, 0, 0};

	RenderQueue queue;
	for (auto [entity, mesh]: scene.reg.view<MeshComponent>().each())
		Enqueue(scene, entity, queue);

	// Light data is shared by every shader, so upload and bind it once for the whole frame.
	scene.light_management.Bind();

	Submit(scene, view, queue);
}

void SceneRenderer::DrawOutline(Scene& scene, const CameraView& view, const std::vector<entt::entity>& meshes,
								const glm::vec3 color, const float width) {
	BR_PROFILE_FUNCTION();

	if (meshes.empty())
		return;

	RenderQueue queue;
	for (const entt::entity mesh: meshes) {
		BR_CORE_ASSERT(scene.reg.all_of<MeshComponent>(mesh),
					   "To draw an outline, the given entities MUST contain a mesh component");
		Enqueue(scene, mesh, queue);
	}

	// Pass 1 - the mask. Redraws the selected meshes exactly as the scene pass did, with
	// every colour write turned off: the point is only to stamp a 1 into the stencil
	// buffer where the object is. Drawing them with their own shader keeps this in step
	// with the main pass for free - vertex positions are all that matter here.
	Command::SetColorWrite(false);
	Command::SetStencil(API::StencilFunction::kAlways, 1, 0xff);

	Submit(scene, view, queue);

	Command::SetColorWrite(true);

	// Pass 2 - the outline itself. The same meshes, pushed outward along their normals and
	// filled with a flat colour, drawn only where pass 1 did not mark the stencil. What
	// survives is the part of the enlarged mesh sticking out past the original: a ring.
	//
	// The stencil write mask is 0 here, so this pass tests against the mask without
	// changing it. Depth testing is off so the ring is never cut into by the geometry it
	// is wrapped around.
	Command::SetStencil(API::StencilFunction::kNotEqual, 1, 0x00);
	Command::DisableDepth();

	const Ref<Shader> outline_shader = ShaderRegistry::GetShader(BR_OUTLINE_SHADER);
	outline_shader->Bind();
	Statistics.Shaders++;

	outline_shader->SetUniformMat4("u_Projection", view.projection);
	outline_shader->SetUniformMat4("u_View", view.view);
	outline_shader->SetUniform3f("u_OutlineColor", color.r, color.g, color.b);
	outline_shader->SetUniform1f("u_OutlineWidth", width);
	Statistics.UniformCalls += 4;

	for (const entt::entity mesh: meshes) {
		if (!scene.IsVisible(mesh))
			continue;

		outline_shader->SetUniformMat4("u_Model", scene.WorldTransform(mesh));

		// The ring covers pixels that belong to whatever is behind it, so it has to claim
		// an id of its own - an output left unwritten leaves the id attachment undefined.
		// Claiming the outlined entity makes clicking the ring select what it wraps.
		// outline_shader->SetUniform1i("u_EntityId", static_cast<u32>(mesh));
		Statistics.UniformCalls++;

		MeshComponent& mesh_component = scene.reg.get<MeshComponent>(mesh);
		const Ref<VertexArray> vao = GetVao(mesh_component, kPhongVertexLayout);

		Command::DrawIndexed(vao, vao->GetIndexBuffer()->GetCount());
		Statistics.Meshes++;
		Statistics.DrawCalls++;
	}

	// Hand the pipeline back the way the scene pass had it.
	Command::DisableStencil();
	Command::EnableDepth();
}
} // namespace bron
