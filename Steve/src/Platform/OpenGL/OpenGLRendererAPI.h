#ifndef OPENGLRENDERERAPI_HEADER
#define OPENGLRENDERERAPI_HEADER

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Renderer/API.h"
#include "Steve/Graphics/VertexArray.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <memory>

namespace Steve
{
	class OpenGLRendererAPI : public API
	{
		static inline int ToOpenGLDrawType(const DrawType type)
		{
			switch (type)
			{
			case DrawType::TRIANGLES:
				return GL_TRIANGLES;
				break;
			case DrawType::LINES:
				return GL_LINES;
				break;
			default:
				CORE_ASSERT(false, "RENDERER: invalid draw type");
			}
			return 0;
		}
	public:
		OpenGLRendererAPI();
		void Init() override;
		void setClearColor(const glm::vec4& color) override;
		void clear() override;
		void onResize(float x, float y, float width, float height) override;

		void EnableBlend() override;
		void EnableDepth() override;

		void drawIndexed(const std::shared_ptr<VertexArray>& v_array, u32 count) override;
		void drawIndexedLines(const std::shared_ptr<VertexArray>& v_array, u32 count) override;
		void drawIndexedStripLines(const std::shared_ptr<VertexArray>& v_array, u32 count) override;

	private:

		bool _blend_enabled;
	};
} // namespace Steve


#endif
