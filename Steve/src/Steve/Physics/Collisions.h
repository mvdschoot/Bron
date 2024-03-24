#ifndef __PHYSICS_COLLISIONS_HEADFER__
#define __PHYSICS_COLLISIONS_HEADFER__

#include <optional>

#include "Components.h"
#include "Math.h"

namespace Steve {

    std::optional<vec3> collides(Pt<RigidBody> a, Pt<RigidBody> b);

}

#endif //__PHYSICS_COLLISIONS_HEADFER__