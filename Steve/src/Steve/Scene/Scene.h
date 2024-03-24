#ifndef _SCENE_HEADER__
#define _SCENE_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/ECS/Registry.h"

#include "Steve/Graphics/Components/Standard.h"
#include "Steve/Graphics/Components/MiscellaneousComponents.h"
#include "Steve/Graphics/Renderer/RenderQueue.h"

#include "Steve/Graphics/Camera.h"
#include "Steve/Graphics/Renderer/2D.h"

// #include "Steve/Physics/PhysicsEngine.h"

namespace Steve
{
	class Scene : public Registry
	{
	public:
		Scene();

		void createStandardModel(const char* name, const char* location);
		void addModel(Model* model);
		StandardCubeComponent* addCube(const char* name, glm::vec3 position, glm::vec3 dimensions);
		void addPointLight(glm::vec3 pos, glm::vec3 color);
		
		Node* root;

		RenderQueue queue;
		std::vector<Model*> allModels;

		std::vector<PointLight*> pointLights;
		Camera* camera;

		// PhysicsEngine* physics;
	private:
	};
}


#endif //_SCENE_HEADER__