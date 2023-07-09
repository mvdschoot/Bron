#include "Scene.h"

namespace Steve::graphics
{
	Scene::Scene()
		: Root(new Entity(&pData))
	{
		RegistryFunctions::AddEntity(&pData, Root);

		float size = 0.1f;
		auto [vertices, normals, indices] = 
			GenCubeSmoothVertices(glm::vec3(- (size / 2.0f) ), glm::vec3(size));
		auto* material = new Material;
		material->Layout = &StandardMaterialLayout;
		material->Data = (u8*)new StandardMaterialBuffer({
			1.0,
			glm::vec3(1.0),
			glm::vec3(1.0),
			200.0f,
			100.0f,
			0.0,0.0,0.0
		});
		material->Textures = nullptr;
		auto* context = new MeshContext(StandardInstances::instance().StandardShader.get(), material);

		// Fill vbo with not just vertices, but also normals & texture coords
		usize stride = StandardVertexLayout->GetStride();
		auto* new_verts = new u8[24 * stride];
		usize vec_size = sizeof(glm::vec3);
		for (int x = 0; x < 24 * stride; x += stride)
		{
			memcpy_s(new_verts + x, vec_size, vertices + (x / stride), vec_size);
			memcpy_s(new_verts + x + vec_size, vec_size, normals + (x / stride), vec_size);
			memset(new_verts + x + (2 * vec_size), 0, sizeof(glm::vec2));
		}

		mPhongPointLightMesh = new Mesh(*StandardVertexLayout, new_verts, 24 * stride, indices, 36, context);

		delete[] new_verts;
		delete[] vertices;
	}

	void Scene::CreateStandardModel(const char* location)
	{
		StandardModelComponent& model = Root->CreateChildEntity(StandardModelComponent(&pData, location));
		AddModel(&model);
	}

	void Scene::AddModel(Model* model)
	{
		Queue.addModel(model);
		AllModels.push_back(model);
	}

	void Scene::AddPointLight(glm::vec3 pos, glm::vec3 color)
	{
		auto *layout = new LightLayout(StandardPointLightLayout);
		const std::string str = "uLight[" + std::to_string(PointLights.size()) + "]";
		layout->SetName(LightData::Position, str + ".Position");
		layout->SetName(LightData::Color, str + ".Color");

		LightContext context{ layout, new u8[layout->GetStride()] };
		context.Set(LightData::Position, (u8*)&pos);
		context.Set(LightData::Color, (u8*)&color);

		PointLight& model = Root->CreateChildEntity(PointLight(&pData, std::move(context)));
		PointLights.push_back(&model);

		model.Meshes.push_back(*mPhongPointLightMesh);

		AddModel(&model);
	}
}
