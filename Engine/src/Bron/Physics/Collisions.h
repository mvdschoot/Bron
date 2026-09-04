#pragma once

#include <optional>

#include "Components.h"
#include "Math.h"

namespace bron {

    std::optional<vec3> Collides(Pt<RigidBody> a, Pt<RigidBody> b);

}
