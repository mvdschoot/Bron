#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_relational.hpp>

#include "Bron/Core/Profiling.h"

namespace bron
{
	class BR_API Camera
	{
	public:
		[[nodiscard]] virtual glm::mat4 GetProjectionMatrix() const = 0;
		[[nodiscard]] virtual glm::mat4 GetViewMatrix() const = 0;
		[[nodiscard]] virtual glm::mat4 GetVPmatrix() const = 0;
		[[nodiscard]] virtual glm::vec3 GetPosition() const = 0;
		[[nodiscard]] virtual glm::vec3 GetDirection() const = 0;
	};
}
