#ifndef __MISCELLANEOUSCOMPONENTS_HEADER__
#define __MISCELLANEOUSCOMPONENTS_HEADER__

#include "Steve/Core/Profiling.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gtc/type_ptr.hpp"

#include "Steve/Util/Util.h"
#include <gtx/quaternion.hpp>


namespace Steve::graphics
{
	struct TransformComponent
	{
		glm::mat4& GetMatrix()
		{
			if (IsDirty())
			{
				OPosition = Position;
				ORotation = Rotation;
				OScaling = Scaling;
				 
				glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

				Matrix = glm::translate(glm::mat4(1.0f), Position)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scaling);
			}
			return Matrix;
		}

		TransformComponent() : Position(0.0),
			Rotation(0.0), Scaling(1.0), Matrix(1.0f),
			OPosition(0.0), ORotation(0.0), OScaling(1.0) {}
		operator glm::mat4&() { return GetMatrix(); }
		glm::mat4& operator*() { return GetMatrix(); }

		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scaling;

	private:
		glm::mat4 Matrix;

		glm::vec3 OPosition;
		glm::vec3 ORotation;
		glm::vec3 OScaling;

		bool IsDirty()
		{
			CH_PROFILE_FUNCTION();
			return !(compare_floats_bits(Position, OPosition)
				&& compare_floats_bits(Rotation, ORotation)
				&& compare_floats_bits(Scaling, OScaling));
		}
	};


}



#endif // __MISCELLANEOUSCOMPONENTS_HEADER__