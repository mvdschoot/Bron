#include "Scene.h"

#include "Steve/Graphics/Components/ModelLoader.h"
#include "Steve/Graphics/Renderer/2D.h"

namespace Steve
{
	Scene::Scene()
		: Registry(), root(new Node(&pData)), lightManagement(&pData)
	{
		root->type |= NodeType_Object;
		root->name = "Root node";
	}

	void Scene::createPhongModel(const char* name, const char* location)
	{
		Ref<Model> model = ModelLoader::loadModel(&pData, MaterialWorkflow::PHONG, location);
		model->name = name;
		root->AddChildNode(std::static_pointer_cast<Node>(model).get(), false);
		allModels.push_back(model);
		queue.addModel(model);
	}

	void Scene::addPointLight(glm::vec3 pos, glm::vec3 color)
	{
		Ref<PointLight> light = lightManagement.createPointLight();
		light->GetComponent<TransformComponent>()->Position = pos;
		light->setColor(color);
		light->name = "Point light " + std::to_string(std::rand());
		root->AddChildNode(std::static_pointer_cast<Node>(light).get(), false);
	}
}
