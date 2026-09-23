//
// Created by mathijs on 8/2/25.
//

#pragma once

#include <entt/entity/registry.hpp>

#include "Bron/Graphics/MaterialBase.h"
#include "Bron/Scene/Components.h"

namespace bron {
class Scene;

/// The geometry of a unit cube, centred on the origin. The engine's built-in cube mesh
/// (assets::builtin::kCubeMesh) is built from this.
MeshData CubeMeshData();

/// Creates a unit cube entity drawing the built-in cube mesh with the given material.
/// Unparented; the caller decides where it goes.
entt::entity CreateCube(Scene& target, const assets::AssetHandle& material = assets::builtin::kDefaultMaterial);

/// Creates a unit cube with a plain phong material of the given colour. The material is a
/// memory-only asset, so it does not survive saving and loading the scene.
entt::entity CreatePhongCube(Scene& target, glm::vec3 color = glm::vec3(1.0f));
} // namespace bron
