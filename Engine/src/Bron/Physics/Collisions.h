#pragma once

#include <optional>

#include "Components.h"
#include "Math.h"

namespace bron {

std::optional<vec3> Collides(Ref<RigidBody> a, Ref<RigidBody> b);

}
