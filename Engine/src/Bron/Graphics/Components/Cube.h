//
// Created by mathijs on 8/2/25.
//

#pragma once

#include <entt/entity/registry.hpp>

#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Scene/Components.h"

namespace bron {
class Scene;

/// Creates a unit cube entity with the given material. Unparented; the caller decides where it goes.
entt::entity CreateCube(Scene& target, const Ref<MaterialBase>& material);

/// Creates a unit cube with a plain phong material of the given colour.
entt::entity CreatePhongCube(Scene& target, glm::vec3 color = glm::vec3(1.0f));
} // namespace bron
