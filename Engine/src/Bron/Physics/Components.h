#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Math.h"

#include <vector>

namespace bron {
/* Internally used by physics engine */
struct P_AABB {
	P_AABB() : min(0.0f), max(0.0f) {}
	P_AABB(vec3 A, vec3 b);

	vec3 min;
	vec3 max;

	bool Contains(P_AABB& other);
	void ResizeTo(P_AABB& other);
};

enum RigidBodyType { kSphere, kCube };

struct RigidBody {
	RigidBodyType type;
	P_AABB bounding_box;

	real weight;
	vec3 center_of_mass;

	bool active;
};

struct SphereBody : public RigidBody {
	float radius;
};

struct CubeBody : public RigidBody {
	// Is equal to the bounding box
};
} // namespace bron
