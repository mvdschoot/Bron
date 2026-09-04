#include "Collisions.h"

#include <memory>

namespace bron {
    #define cast std::static_pointer_cast

    std::optional<vec3> Collides(Pt<RigidBody> a, Pt<RigidBody> b)
    {

        const RigidBodyType at = a->type;
        const RigidBodyType bt = b->type;

        if (at == RigidBodyType::SPHERE && bt == RigidBodyType::SPHERE) {
            return Collides(cast<SphereBody>(a), cast<SphereBody>(b));
        }

        return {};
    }

    std::optional<vec3> Collides(Pt<SphereBody> a, Pt<SphereBody> b) {
        return {};
    }
}