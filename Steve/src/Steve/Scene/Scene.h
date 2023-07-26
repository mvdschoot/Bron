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

#include "Node.h"

namespace Steve::graphics
{
	class Scene : public Registry
	{
	public:
		Scene();

		void CreateStandardModel(const char* name, const char* location);
		void AddModel(Model* model);
		StandardCubeComponent* AddCube(const char* name, glm::vec3 position, glm::vec3 dimensions);

		void AddPointLight(glm::vec3 pos, glm::vec3 color);
		
		Entity* Root;

		RenderQueue Queue;
		std::vector<Model*> AllModels;

		std::vector<PointLight*> PointLights;
		Camera* Camera;
	private:
	};
}


#endif //_SCENE_HEADER__