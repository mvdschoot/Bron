//
// Created by mathijs on 8/2/25.
//

#ifndef PHONGCUBE_H
#define PHONGCUBE_H

#include "PhongMaterial.h"
#include "Steve/Graphics/Components/Cube.h"

namespace Steve {

class PhongCube : public Cube {
	public:
	explicit PhongCube(RegistryData *reg) : Cube(reg) {}

	glm::vec3 GetColor() const override;
	void SetColor(glm::vec3 color) override;

	protected:
		Ref<MaterialBase> generateMaterial() override;

	private:
		Ref<PhongMaterial> material;
	};

} // Steve

#endif //PHONGCUBE_H
