//
// Created by mathijs on 8/3/25.
//

#include "LightManagement.h"

namespace Steve {
	void LightManagement::bind() {
		// Check if the light data has been updated.
		bool isUpdated = false;
		for (auto& pointlight : pointlights) {
			isUpdated |= pointlight->isUpdatedAndReset();
		}

		if (isUpdated) {
			generateUbo();
		}

		if (!pointlightsUbo->isBound()) {
			pointlightsUbo->bind(POINTLIGHT_UBO_INDEX);
		}
	}

	Ref<PointLight> LightManagement::createPointLight() {
		auto light = Ref<PointLight>(new PointLight(regData));
		pointlights.push_back(light);
		isPointlightAdded = true;
		return light;
	}

	void LightManagement::generateUbo() {
		std::vector<ShaderPointlightStruct> lights;
		for (auto& pointlight : pointlights) {
			ShaderPointlightStruct& lightStruct = lights.emplace_back();
			lightStruct.position = pointlight->GetComponent<TransformComponent>()->Position;
			lightStruct.color = pointlight->getColor();
		}

		constexpr usize lightDataSize = sizeof(ShaderPointlightStruct);
		constexpr usize totalSize = lightDataSize * POINTLIGHT_MAX;
		u8 *lightData = new u8[totalSize];
		memset(lightData, 0, totalSize);
		memcpy(lightData, &lights[0], lightDataSize * lights.size());

		pointlightsUbo = UniformBuffer::Create(lightData, totalSize, POINTLIGHT_UBO_INDEX);

	}
} // Steve