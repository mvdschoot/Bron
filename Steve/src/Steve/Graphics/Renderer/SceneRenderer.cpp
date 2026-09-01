#include "SceneRenderer.h"

#include "Command.h"
#include "Steve/Graphics/Phong/PhongDefinitions.h"
#include "Steve/Graphics/ShaderRegistry.h"

#include <map>
#include <string>


namespace Steve
{
	RenderStatistics SceneRenderer::Statistics = {};

	struct SceneRendererData
	{
		Ref<Texture> whiteTexture;

		static constexpr u32 textureSlots = 32;
		u32 textureArray[textureSlots]{};

	};
	static SceneRendererData Data;

	void SceneRenderer::Init()
	{
		Data.whiteTexture = Texture2D::Create(1, 1);
		u32 white_data = 0xffffffff;
		Data.whiteTexture->setData(&white_data, sizeof(u32));

		for(int i = 0; i < Steve::SceneRendererData::textureSlots; i++)
		{
			Data.textureArray[i] = i;
		}
	}

	void SceneRenderer::Draw(Scene& scene)
	{
		CH_PROFILE_FUNCTION();

		Statistics = { 0,0,0,0,0 };

		CORE_ASSERT(scene.camera != nullptr, "This scene has no camera attached")

		// Sort the drawable entities by shader, then by material, so each is only bound once.
		// Rebuilt every frame: the registry is the single source of truth, and a scene of this size
		// makes the sort free compared to keeping a second container in sync.
		std::map<std::string, std::map<MaterialBase*, std::vector<entt::entity>>> queue;
		for (auto [entity, mesh] : scene.reg.view<MeshComponent>().each())
		{
			CORE_ASSERT(mesh.material != nullptr, "Mesh has no material and cannot be drawn")
			queue[mesh.material->shaderName][mesh.material.get()].push_back(entity);
		}

		for (auto& [shader_name, materials] : queue)
		{
			Statistics.Shaders++;
			Ref<Shader> shader = ShaderRegistry::GetShader(shader_name.c_str());
			shader->bind();

			shader->setUniform1iv("u_Textures", (i32*)Data.textureArray, Data.textureSlots);
			shader->setUniformMat4("u_Projection", scene.camera->GetProjectionMatrix());
			shader->setUniformMat4("u_View", scene.camera->GetViewMatrix());
			shader->setUniform3f("u_ViewPos", scene.camera->GetPosition().x, scene.camera->GetPosition().y, scene.camera->GetPosition().z);
			Statistics.UniformCalls += 3;

			scene.lightManagement.bind();
			shader->setUniform1i("u_NumPointLights", scene.lightManagement.numberPointLights());

			for (auto& [material, entities] : materials)
			{
				Statistics.Materials++;

				Data.whiteTexture->bind(0);

				material->Bind(shader, 1);
				Statistics.UniformCalls += material->NumberUniformCalls();

				for (const entt::entity entity : entities)
				{
					MeshComponent& mesh = scene.reg.get<MeshComponent>(entity);

					shader->setUniformMat4("u_Model", scene.WorldTransform(entity));
					Statistics.UniformCalls++;

					const Ref<VertexArray> vao = GetVao(mesh, PhongVertexLayout);
					Command::DrawIndexed(vao, vao->getIndexBuffer()->getCount());

					Statistics.DrawCalls++;
					Statistics.Meshes++;
				}
			}
		}
	}
}
