//
// Created by mathijs on 8/3/25.
//

#ifndef LIGHTMANAGEMENT_H
#define LIGHTMANAGEMENT_H
#include "Components/PointLight.h"

#define POINTLIGHT_UBO_INDEX 0

#define POINTLIGHT_MAX 32

namespace Steve {

class LightManagement {
public:
	struct ShaderPointlightStruct {
		glm::vec3 position{0.0f};
		float padding1;

		glm::vec3 color{1.0f};
		float padding2;
	};

	explicit LightManagement(RegistryData* regData) : regData(regData) {};
	~LightManagement() = default;

	void bind();
	Ref<PointLight> createPointLight();
	[[nodiscard]] u8 numberPointLights() const { return pointlights.size();}
private:
	void generateUbo();

	RegistryData* regData;

	std::vector<Ref<PointLight>> pointlights;
	Ref<UniformBuffer> pointlightsUbo;

	bool isPointlightAdded = false;
};

} // Steve

#endif //LIGHTMANAGEMENT_H
