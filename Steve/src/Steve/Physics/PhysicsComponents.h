#ifndef __PHCOMPONENTS_HEADER__
#define __PHCOMPONENTS_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/Logger.h"

#include "Steve/ECS/Entity.h"

#include "Steve/Graphics/Components/MiscellaneousComponents.h"

#include "Steve/Scene/Node.h"

#include <vector>

#include <glm/glm.hpp>


namespace Steve
{ 
	struct CollisionBody : public Node
	{
		bool Active = true;

		// We assume Transform.Position = centroid.
		Handle<TransformComponent>& Transform;
	};



	/* Internally used by physics engine */
	struct AABB
	{
		AABB() : Min(0.0f), Max(0.0f) {}
		AABB(glm::vec3 A, glm::vec3 B);

		glm::vec3 Min;
		glm::vec3 Max;

		bool Contains(AABB& other);
		void ResizeTo(AABB& other);
	};
	
	struct CollisionNode
	{
		AABB Box;

		// Has body if it is a leaf
		CollisionBody* Body;

		CollisionNode* Left, * Right;

		std::vector<CollisionNode*> Primitives;
	};

	struct SphereCollisionBody : public CollisionBody
	{
		float Radius;
	};

	// == AABB 
	struct CubeCollisionBody : public CollisionBody
	{
		glm::vec3 Dimensions;
	};

	struct TriangleCollisionBody : public CollisionBody
	{
		
	};
}

#endif