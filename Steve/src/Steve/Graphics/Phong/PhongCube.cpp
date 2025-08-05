//
// Created by mathijs on 8/2/25.
//

#include "PhongCube.h"

#include "PhongMaterial.h"

namespace Steve {
	glm::vec3 PhongCube::GetColor() const { return *material->Get<glm::vec3>(PhongMaterialVariables::Diffuse); }

	void PhongCube::SetColor(glm::vec3 color) {
		material->Set(PhongMaterialVariables::Diffuse, color);
		material->Set(PhongMaterialVariables::Specular, color);
	}

	Ref<MaterialBase> PhongCube::generateMaterial() {
		const Ref<PhongMaterial> material = createRef<PhongMaterial>();
		this->material = material;
		SetColor(glm::vec3(1.0f));
		return std::static_pointer_cast<MaterialBase>(material);
	}
} // Steve