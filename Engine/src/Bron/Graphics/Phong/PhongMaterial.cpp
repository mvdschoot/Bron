//
// Created by mathijs on 8/1/25.
//


#include "PhongMaterial.h"

#include <utility>

namespace Bron {
    u8 PhongMaterial::Bind(const Ref<Shader> shader, u8 textureSlotStartingIndex) {
		if (!shader->isBound()) {
			shader->bind();
		}

		// Reset the textures
		Set(PhongMaterialVariables::DiffuseTexture, 0.0f);
		Set(PhongMaterialVariables::SpecularTexture, 0.0f);
		// Set(PhongMaterialVariables::NormalTexture, 0);

		// Bind the available textures to texture slots
		int textureSlotIndex = textureSlotStartingIndex;
		if (textures.contains(PhongMaterialTextureTypes::Diffuse)) {
			textures.at(PhongMaterialTextureTypes::Diffuse)->bind(textureSlotIndex);
			Set(PhongMaterialVariables::DiffuseTexture, static_cast<float>(textureSlotIndex));
			textureSlotIndex += 1;
		}
		if (textures.contains(PhongMaterialTextureTypes::Specular)) {
			textures.at(PhongMaterialTextureTypes::Specular)->bind(textureSlotIndex);
			Set(PhongMaterialVariables::SpecularTexture, static_cast<float>(textureSlotIndex));
			textureSlotIndex += 1;
		}
		// if (textures.contains(PhongMaterialTextureTypes::Normal)) {
		// 	textures.at(PhongMaterialTextureTypes::Normal)->bind(textureSlotIndex);
		// 	Set(PhongMaterialVariables::NormalTexture, static_cast<float>(textureSlotIndex));
		// 	textureSlotIndex += 1;
		// }

		shader->setUniforms(*Layout, Data);

		return textureSlotIndex;
	}

	u32 PhongMaterial::NumberUniformCalls() const { return Layout->Data.size(); }

	void PhongMaterial::AddTexture(PhongMaterialTextureTypes textureType, Ref<Texture> texture) {
	    textures[textureType] = std::move(texture);
    }
} // namespace Bron
