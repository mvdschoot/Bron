#include "Scene.h"

#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"

namespace Steve::graphics
{
	Scene::Scene()
		: Registry(), Root(new Entity(&pData))
	{}

	void Scene::CreateStandardModel(const char* name, const char* location)
	{
		StandardModelComponent& model = Root->CreateChildEntity(StandardModelComponent(&pData, location));
		model.Name = name;
		AddModel(&model);
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
		t.Set(SET_TRANSLATE | SET_SCALING, { position, dimensions });
		model->Name = name;

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

		PointLight& model = Root->CreateChildEntity(PointLight(&pData, std::move(data)));
		model.Name = "Pointlight " + std::to_string(PointLights.size());
		PointLights.push_back(&model);

		Queue.addModel(&model);

		model.GetComponent<TransformComponent>()->Set(SET_TRANSLATE, { pos });
		model.SetUniformPosition(pos);
		model.SetColor(color);
	}
}
