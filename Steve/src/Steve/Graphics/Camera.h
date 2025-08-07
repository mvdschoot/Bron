#ifndef CAMERA_HEADER
#define CAMERA_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_relational.hpp>

#include "Steve/Core/Profiling.h"

namespace Steve
{
	class STEVE_API Camera
	{
	public:
		[[nodiscard]] virtual glm::mat4 GetProjectionMatrix() const = 0;
		[[nodiscard]] virtual glm::mat4 GetViewMatrix() const = 0;
		[[nodiscard]] virtual glm::mat4 GetVPmatrix() const = 0;
		[[nodiscard]] virtual glm::vec3 GetPosition() const = 0;
		[[nodiscard]] virtual glm::vec3 GetDirection() const = 0;
	};
}

#endif
