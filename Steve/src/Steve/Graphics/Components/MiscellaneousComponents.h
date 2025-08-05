#ifndef __MISCELLANEOUSCOMPONENTS_HEADER__
#define __MISCELLANEOUSCOMPONENTS_HEADER__

#include "Steve/Core/Profiling.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// #include "glm/gtc/type_ptr.hpp"

#include "Steve/Util/Util.h"
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Steve
{
	struct TransformComponent
	{
		glm::mat4& GetMatrix()
		{
			if (IsDirty())
			{
				OPosition = Position;
				ORotationQuat = RotationQuat;
				OScaling = Scaling;
				 
				glm::mat4 rotation = glm::toMat4(glm::quat(RotationQuat));

				Matrix = glm::translate(glm::mat4(1.0f), Position)
					* rotation
					* glm::scale(glm::mat4(1.0f), Scaling);
			}
			return Matrix;
		}

		void SyncEulerFromQuat()
		{
			EulerCache = glm::degrees(glm::eulerAngles(RotationQuat));
		}

		void SyncQuatFromEuler()
		{
			RotationQuat = glm::quat(glm::radians(EulerCache));
		}

		bool IsDirty() const
		{
			CH_PROFILE_FUNCTION();
			return !(compare_floats_bits(Position, OPosition)
				&& compare_floats_bits((glm::vec4*)(&RotationQuat), (glm::vec4*)(&ORotationQuat))
				&& compare_floats_bits(Scaling, OScaling));
		}

		TransformComponent() : Position(0.0),
			RotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), Scaling(1.0), Matrix(1.0f),
			OPosition(0.0), ORotationQuat({1.0f, 0.0f, 0.0f, 0.0f}), OScaling(1.0) {}
		operator glm::mat4&() { return GetMatrix(); }
		glm::mat4& operator*() { return GetMatrix(); }

		glm::vec3 Position;
		glm::quat RotationQuat; // w,x,y,z
		glm::vec3 Scaling;

		glm::vec3 EulerCache{0.0f}; // purely for ImGui UI

	private:
		glm::mat4 Matrix;

		glm::vec3 OPosition;
		glm::quat ORotationQuat;
		glm::vec3 OScaling;

	};


}



#endif // __MISCELLANEOUSCOMPONENTS_HEADER__