#include "SceneRenderer.h"

#include "Command.h"


namespace Steve::graphics
{
	RenderStatistics SceneRenderer::Statistics = {};

	struct SceneRendererData
	{
		Ref<Texture> whiteTexture;

		static constexpr u32 textureSlots = 32;
		u32 textureArray[textureSlots];

	};
	static SceneRendererData Data;

	void SceneRenderer::Init()
	{
		Data.whiteTexture = Texture2D::Create(1, 1);
		u32 white_data = 0xffffffff;
		Data.whiteTexture->setData(&white_data, sizeof(u32));

		for(int i = 0; i < Data.textureSlots; i++)
		{
			Data.textureArray[i] = i;
		}
	}

	void SceneRenderer::Draw(Scene& scene)
	{
		Statistics = { 0,0,0,0,0,0 };

		CORE_ASSERT(scene.Camera != nullptr, "This scene has no camera attached")

		for (auto& [shader, set] : scene.Queue.pQueue.pSet)
		{
			Statistics.Shaders++;
			shader->bind();

			shader->setUniform1iv("uTextures", (i32*)Data.textureArray, Data.textureSlots);
			shader->setUniformMat4("uVPmatrix", scene.Camera->GetVPmatrix());
			shader->setUniformMat4("uViewMatrix", scene.Camera->GetViewMatrix());
			Statistics.UniformCalls += 3;
			
			for(int x = 0; x < scene.PointLights.size(); x++)
			{
				TransformComponent& t = scene.PointLights[x]->GetComponent<TransformComponent>();
				LightData& l = scene.PointLights[x]->GetComponent<LightData>();
				shader->setUniforms(*l.Layout, l.Data);
				Statistics.UniformCalls += l.Layout->Data.size();
			}

			for (auto& [model, materials] : set.pSet) {
				Statistics.Models++;

				glm::mat4 modelTransform(1.0f);
				if (model->Contains<TransformComponent>())
				{
					modelTransform = **model->GetComponent<TransformComponent>();
				}

				for (auto& [material, meshes] : materials.pSet)
				{
					Statistics.Materials++;

					material->Set(MaterialDataTypes::DiffuseTexture, 0);
					material->Set(MaterialDataTypes::SpecularTexture, 0);
					material->Set(MaterialDataTypes::NormalTexture, 0);
					Data.whiteTexture->bind(0);

					if (material->Textures != nullptr) {
						u16 count = 1;
						for (auto& [type, texture] : material->Textures->Textures)
						{
							texture->bind(count);
							material->Set(TextureConverter(type), (float)count);
							count++;
						}
					}

					shader->setUniforms(*material->Layout, material->Data);
					Statistics.UniformCalls += material->Layout->Data.size();

					for (Mesh* mesh : meshes)
					{
						glm::mat4 meshTransform = modelTransform;
						if (mesh->Contains<TransformComponent>())
						{
							meshTransform *= **mesh->GetComponent<TransformComponent>();
						}

						shader->setUniformMat4("uTransform", meshTransform);
						Statistics.UniformCalls++;

						Command::DrawIndexed(mesh->pVao, mesh->pVao->getIndexBuffer()->getCount());

						Statistics.DrawCalls++;
						Statistics.Meshes++;
					}
				}
			}
		}
	}
}
