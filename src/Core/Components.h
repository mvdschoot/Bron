#ifndef __APP_COMPONENTS_HEADER__
#define __APP_COMPONENTS_HEADER__

#include "Cheets.h"

namespace Cheets
{
	struct TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		TranformComponent(float x, float y, float z) : m_Transform(x, y, z)
		{}

		glm::vec3 m_Transform;
	};
}

#endif