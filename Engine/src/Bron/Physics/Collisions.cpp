#include "Collisions.h"

#include <memory>

namespace bron {
#define cast std::static_pointer_cast

std::optional<vec3> Collides(Ref<RigidBody> a, Ref<RigidBody> b) {

	const RigidBodyType at = a->type;
	const RigidBodyType bt = b->type;

	if (at == RigidBodyType::kSphere && bt == RigidBodyType::kSphere) {
		return Collides(cast<SphereBody>(a), cast<SphereBody>(b));
	}

	return {};
}

std::optional<vec3> Collides(Ref<SphereBody> a, Ref<SphereBody> b) { return {}; }
} // namespace bron
