#include "Scene.h"

#include "Steve/Graphics/Renderer/2D.h"

namespace Steve
{
	Scene::Scene()
		: Registry(), root(new Node(&pData)) //, physics(new PhysicsEngine())
	{
		root->type |= NodeType_Object;
		root->name = "Root node";
	}

	void Scene::createStandardModel(const char* name, const char* location)
	{
		StandardModelComponent* model = new StandardModelComponent(&pData, location);
		root->AddChildNode(model, true);
		model->name = name;
		addModel(model);
	}

	void Scene::addModel(Model* model)
	{
		queue.addModel(model);
		allModels.push_back(model);
	}

	StandardCubeComponent* Scene::addCube(const char* name, glm::vec3 position, glm::vec3 dimensions)
	{
		auto* model = new StandardCubeComponent(&pData);
		TransformComponent& t = *model->GetComponent<TransformComponent>();
		t.Position = position;
		t.Scaling = dimensions;
		model->name = name;
		root->AddChildNode(model, true);

		queue.addModel(model);
		allModels.push_back(model);
		return model;
	}

	void Scene::addPointLight(glm::vec3 pos, glm::vec3 color)
	{
		auto *layout = new UniformLayout(StandardPointLightLayout);
		const std::string str = "uLight[" + std::to_string(pointLights.size()) + "]";
		layout->SetName(LightDataTypes::Position, str + ".Position");
		layout->SetName(LightDataTypes::Color, str + ".Color");

		LightData data(layout);

		PointLight* model = new PointLight(&pData, std::move(data));
		model->name = "Pointlight " + std::to_string(pointLights.size());
		root->AddChildNode(model);
		pointLights.push_back(model);

		queue.addModel(model);

		model->GetComponent<TransformComponent>()->Position = pos;
		LightData& d = *model->GetComponent<LightData>();
		model->SetUniformPosition(pos);
		model->SetColor(color);
	}
}
