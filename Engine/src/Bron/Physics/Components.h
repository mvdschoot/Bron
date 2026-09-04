#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Math.h"

#include <vector>

namespace bron
{
	/* Internally used by physics engine */
	struct AABB
	{
		AABB() : min(0.0f), max(0.0f) {}
		AABB(vec3 A, vec3 B);

		vec3 min;
		vec3 max;

		bool Contains(AABB& other);
		void ResizeTo(AABB& other);
	};

	enum RigidBodyType {
		kSphere,
		kCube
	};

	struct RigidBody {
		RigidBodyType type;
		AABB bounding_box;
		
		real weight;
		vec3 center_of_mass;

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
