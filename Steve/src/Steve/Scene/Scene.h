#ifndef _SCENE_HEADER__
#define _SCENE_HEADER__

#include "Steve/ECS/Registry.h"

#include "Steve/Graphics/Components/MiscellaneousComponents.h"
#include "Steve/Graphics/Renderer/RenderQueue.h"

#include "Steve/Graphics/Camera.h"
#include "Steve/Graphics/LightManagement.h"

namespace Steve
{
	class Scene : public Registry
	{
	public:
		Scene();

		void createPhongModel(const char* name, const char* location);
		void addPointLight(glm::vec3 pos, glm::vec3 color);
		
		Node* root;

		RenderQueue queue;
		std::vector<Ref<Model>> allModels;

		LightManagement lightManagement;
		Camera* camera;
	private:
	};
}


#endif //_SCENE_HEADER__