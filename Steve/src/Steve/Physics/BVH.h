#ifndef __BVH_HEADER__
#define __BVH_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/Logger.h"

#include "Components.h"
#include "Defs.h"

#include <vector>

namespace Steve
{
	struct PhysicsData;

	struct BvhNode {
		AABB box;
		Pt<BvhNode> left, right;
		std::vector<Pt<RigidBody>> primitives;
	};

	class BVH
	{
	private:
		static constexpr u8 MAX_PRIMS_IN_NODE = 5;

	public:
		BVH();

		void addObject(Pt<RigidBody> obj);
		std::vector<Pt<RigidBody>>& get(Pt<RigidBody>& object);

	private:
		void insertPrimitive(Pt<BvhNode> current, Pt<RigidBody> n);
		Pt<BvhNode> find(Pt<RigidBody> toFind);

		Pt<BvhNode> root;
	};
}

#endif