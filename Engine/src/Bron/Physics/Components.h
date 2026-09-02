#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Math.h"

#include <vector>

namespace Bron
{
	/* Internally used by physics engine */
	struct AABB
	{
		AABB() : min(0.0f), max(0.0f) {}
		AABB(vec3 A, vec3 B);

		vec3 min;
		vec3 max;

		bool contains(AABB& other);
		void resizeTo(AABB& other);
	};

	enum RigidBodyType {
		SPHERE,
		CUBE
	};

	struct RigidBody {
		RigidBodyType type;
		AABB boundingBox;
		
		real weight;
		vec3 centerOfMass;

		bool active;
	};

	struct SphereBody : public RigidBody
	{
		float radius;
	};

	struct CubeBody : public RigidBody
	{
		// Is equal to the bounding box
	};
}
