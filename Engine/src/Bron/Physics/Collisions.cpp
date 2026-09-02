#include "Collisions.h"

#include <memory>

namespace Bron {
    #define cast std::static_pointer_cast

    std::optional<vec3> collides(Pt<RigidBody> a, Pt<RigidBody> b)
    {

        const RigidBodyType at = a->type;
        const RigidBodyType bt = b->type;

        if (at == RigidBodyType::SPHERE && bt == RigidBodyType::SPHERE) {
            return collides(cast<SphereBody>(a), cast<SphereBody>(b));
        }

        return {};
    }

    std::optional<vec3> collides(Pt<SphereBody> a, Pt<SphereBody> b) {
        return {};
    }
}