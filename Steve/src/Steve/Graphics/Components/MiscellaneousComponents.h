#ifndef __MISCELLANEOUSCOMPONENTS_HEADER__
#define __MISCELLANEOUSCOMPONENTS_HEADER__

#include <glm/glm.hpp>

namespace Steve::graphics
{
	template<typename T>
	struct ComponentInterface
	{
		virtual operator T& () = 0;
		virtual T& operator*() = 0;
	};

	struct TransformComponent : ComponentInterface<glm::mat4>
	{
		glm::mat4 Transform;

		TransformComponent(glm::mat4 mat) : Transform(mat) {}
		operator glm::mat4&() override { return Transform; }
		glm::mat4& operator*() override { return Transform; }
	};
}



#endif // __MISCELLANEOUSCOMPONENTS_HEADER__