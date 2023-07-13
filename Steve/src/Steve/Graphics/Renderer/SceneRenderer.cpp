#include "SceneRenderer.h"

#include "Command.h"


namespace Steve::graphics
{
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
		CORE_ASSERT(scene.Camera != nullptr, "This scene has no camera attached")

		for (auto& [shader, set] : scene.Queue.pQueue.pSet)
		{
			shader->bind();


			shader->setUniform1iv("uTextures", (i32*)Data.textureArray, Data.textureSlots);
			shader->setUniformMat4("uVPmatrix", scene.Camera->GetVPmatrix());
			shader->setUniformMat4("uViewMatrix", scene.Camera->GetViewMatrix());
			
			for(int x = 0; x < scene.PointLights.size(); x++)
			{
				LightData& l = scene.PointLights[x]->GetComponent<LightData>();
				shader->setUniforms(*l.Layout, l.Data);
			}

			for (auto& [model, materials] : set.pSet) {

				if (model->Contains<TransformComponent>())
				{
					shader->setUniformMat4("uTransform", *model->GetComponent<TransformComponent>());
				} else
				{
					shader->setUniformMat4("uTransform", glm::mat4(1.0f));
				}

				for (auto& [material, meshes] : materials.pSet)
				{
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
					for (Mesh* mesh : meshes)
					{
						Command::DrawIndexed(mesh->pVao, mesh->pVao->getIndexBuffer()->getCount());
					}
				}
			}
		}
	}
}
