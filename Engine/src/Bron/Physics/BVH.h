#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Components.h"
#include "Defs.h"

#include <vector>

namespace bron
{
	struct PhysicsData;

	struct BvhNode {
		AABB box;
		Ref<BvhNode> left, right;
		std::vector<Ref<RigidBody>> primitives;
	};

	class BVH
	{
	private:
		static constexpr u8 MAX_PRIMS_IN_NODE = 5;

	public:
		BVH();

		void AddObject(Ref<RigidBody> obj);
		std::vector<Ref<RigidBody>>& get(Ref<RigidBody>& object);

	private:
		void InsertPrimitive(Ref<BvhNode> current, Ref<RigidBody> n);
		Ref<BvhNode> find(Ref<RigidBody> to_find);

		Ref<BvhNode> root;
	};
}
