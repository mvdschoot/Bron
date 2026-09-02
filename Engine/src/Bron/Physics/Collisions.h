#pragma once

#include <optional>

#include "Components.h"
#include "Math.h"

namespace Bron {

    std::optional<vec3> collides(Pt<RigidBody> a, Pt<RigidBody> b);

}
