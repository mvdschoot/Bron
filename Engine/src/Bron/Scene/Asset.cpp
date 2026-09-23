//
// Created by mathi on 22-9-2026.
//

#include "Asset.h"

#include <cstring>

namespace bron::assets {
AssetHandle FixedHandle(const char* name) {
	AssetHandle handle;
	BR_CORE_ASSERT(std::strlen(name) < sizeof(handle.value), "Fixed handle '{}' does not fit in a UUID", name);

	std::strncpy(handle.value, name, sizeof(handle.value) - 1);
	handle.value[sizeof(handle.value) - 1] = '\0';
	return handle;
}

MeshAsset::MeshAsset(MeshData&& mesh_data) :
	mesh_data(std::move(mesh_data)), vao(GenerateVao(this->mesh_data)), aabb(GenerateAABB(this->mesh_data)) {}
} // namespace bron::assets
