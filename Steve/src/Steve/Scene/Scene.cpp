#include "Scene.h"

#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"

namespace Steve
{
	Scene::Scene()
		: Registry(), Root(new Node(&pData))
	{

		Root->type |= NodeType_Object;
		Root->name = "Root node";
	}

	void Scene::CreateStandardModel(const char* name, const char* location)
	{
		StandardModelComponent* model = new StandardModelComponent(&pData, location);
		Root->AddChildNode(model, true);
		model->name = name;
		AddModel(model);
	}

	void Scene::AddModel(Model* model)
	{
		Queue.addModel(model);
		AllModels.push_back(model);
	}

	StandardCubeComponent* Scene::AddCube(const char* name, glm::vec3 position, glm::vec3 dimensions)
	{
		auto* model = new StandardCubeComponent(&pData);
		TransformComponent& t = *model->GetComponent<TransformComponent>();
		t.Position = position;
		t.Scaling = dimensions;
		model->name = name;
		Root->AddChildNode(model, true);

		Queue.addModel(model);
		AllModels.push_back(model);
		return model;
	}

	void Scene::AddPointLight(glm::vec3 pos, glm::vec3 color)
	{
		auto *layout = new UniformLayout(StandardPointLightLayout);
		const std::string str = "uLight[" + std::to_string(PointLights.size()) + "]";
		layout->SetName(LightDataTypes::Position, str + ".Position");
		layout->SetName(LightDataTypes::Color, str + ".Color");

		LightData data(layout);

		PointLight* model = new PointLight(&pData, std::move(data));
		model->name = "Pointlight " + std::to_string(PointLights.size());
		Root->AddChildNode(model);
		PointLights.push_back(model);

		Queue.addModel(model);

		model->GetComponent<TransformComponent>()->Position = pos;
		LightData& d = *model->GetComponent<LightData>();
		model->SetUniformPosition(pos);
		model->SetColor(color);
	}
}
