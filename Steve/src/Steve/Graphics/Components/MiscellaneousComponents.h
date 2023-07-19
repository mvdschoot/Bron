#ifndef __MISCELLANEOUSCOMPONENTS_HEADER__
#define __MISCELLANEOUSCOMPONENTS_HEADER__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "gtc/type_ptr.hpp"

#include <ImGuizmo.h>


namespace Steve::graphics
{
	struct TransformComponent
	{
#define SET_TRANSLATE 1 << 0
#define SET_ROTATION 1 << 1
#define SET_SCALING 1 << 2
#define SET_ALL3 SET_TRANSLATE | SET_ROTATION | SET_SCALING

		glm::mat4 Matrix;

		TransformComponent() : Matrix(1.0f) {}
		operator glm::mat4&() { return Matrix; }
		glm::mat4& operator*() { return Matrix; }

		void Set(u8 to_set, std::initializer_list<glm::vec3> data)
		{
			float t[3], r[3], s[3];
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(Matrix), t, r, s);

			u8 count = 0;
			if (to_set & SET_TRANSLATE)
			{
				memcpy_s(t, sizeof(glm::vec3), (void*)data.begin(), sizeof(glm::vec3));
				count++;
			}
			if (to_set & SET_ROTATION)
			{
				memcpy_s(r, sizeof(glm::vec3), (void*)(data.begin() + count), sizeof(glm::vec3));
				count++;
			}
			if (to_set & SET_SCALING)
			{
				memcpy_s(s, sizeof(glm::vec3), (void*)(data.begin() + count), sizeof(glm::vec3));
			}

			ImGuizmo::RecomposeMatrixFromComponents(t, r, s, glm::value_ptr(Matrix));
		}

		// Translate, Rotate, Scale
		std::tuple<glm::vec3, glm::vec3, glm::vec3> Get() const
		{
			float t[3], r[3], s[3];
			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(Matrix), t, r, s);
			return { { t[0], t[1], t[2] }, { r[0],r[1],r[2] }, { s[0],s[1],s[2] } };
		}
	};


}



#endif // __MISCELLANEOUSCOMPONENTS_HEADER__