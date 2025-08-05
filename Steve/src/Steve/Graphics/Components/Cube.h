//
// Created by mathijs on 8/2/25.
//

#ifndef CUBE_H
#define CUBE_H
#include "Model.h"

namespace Steve {
	class Cube : public Model
	{
	public:
		virtual glm::vec3 GetColor() const = 0;
		virtual void SetColor(glm::vec3 color) = 0;

	protected:
		explicit Cube(RegistryData* reg) : Model(reg, { generateMesh(reg) }) {}

		virtual Ref<MaterialBase> generateMaterial() = 0;

	private:
		Ref<Mesh> generateMesh(RegistryData* reg);

		Ref<Mesh> mesh;
	};
}

#endif //CUBE_H
