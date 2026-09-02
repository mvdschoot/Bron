#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Components.h"
#include "Defs.h"

#include <vector>

namespace Bron
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
