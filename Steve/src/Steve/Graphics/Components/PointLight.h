//
// Created by mathijs on 8/3/25.
//

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Cube.h"
#include "Steve/Core/Updatable.h"


namespace Steve {

	class PointLight final : public Node, public Updatable {
	public:
		PointLight(RegistryData *regData) :
				Node(regData),
				Updatable([this]() {return this->GetComponent<TransformComponent>()->IsDirty();}),
				color(1.0f) {
			type |= NodeType_PointLight;
		}

		void setColor(glm::vec3 color) { this->color = color; areVariablesUpdated = true; }
		glm::vec3 &getColor() { return color; }

	protected:
		glm::vec3 color;
	};

} // Steve

#endif //POINTLIGHT_H
