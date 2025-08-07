#include "SceneRenderer.h"

#include "Command.h"
#include "Steve/Graphics/Phong/PhongDefinitions.h"
#include "Steve/Graphics/ShaderRegistry.h"


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
		Statistics = { 0,0,0,0,0 };

		CORE_ASSERT(scene.camera != nullptr, "This scene has no camera attached")

		for (auto& [shader_name, set] : scene.queue.pQueue)
		{
			Statistics.Shaders++;
			Ref<Shader> shader = ShaderRegistry::GetShader(shader_name);
			shader->bind();

			shader->setUniform1iv("u_Textures", (i32*)Data.textureArray, Data.textureSlots);
			shader->setUniformMat4("u_Projection", scene.camera->GetProjectionMatrix());
			shader->setUniformMat4("u_View", scene.camera->GetViewMatrix());
			shader->setUniform3f("u_ViewPos", scene.camera->GetPosition().x, scene.camera->GetPosition().y, scene.camera->GetPosition().z);
			Statistics.UniformCalls += 3;
			
			scene.lightManagement.bind();
			shader->setUniform1i("u_NumPointLights", scene.lightManagement.numberPointLights());

			for (auto& [material, meshes] : set)
			{
				Statistics.Materials++;

				Data.whiteTexture->bind(0);

				material->Bind(shader, 1);
				Statistics.UniformCalls += material->NumberUniformCalls();

				for (Ref<Mesh>& mesh : meshes)
				{
					glm::mat4 t = mesh->GetTransform();

					shader->setUniformMat4("u_Model", t);
					Statistics.UniformCalls++;

					Command::DrawIndexed(mesh->GetVao(PhongVertexLayout), mesh->GetVao(PhongVertexLayout)->getIndexBuffer()->getCount());

					Statistics.DrawCalls++;
					Statistics.Meshes++;
				}
			}
		}
	}
}
