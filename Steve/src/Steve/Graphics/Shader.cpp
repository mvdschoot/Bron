#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Steve/Core/Platform.h"

namespace Steve
{
	Ref<Shader> Shader::CreateShaderFromLocation(const std::string& shader_loc)
	{
		std::ifstream a(shader_loc);
		CORE_ASSERT(!a.fail(), "Shaderloc does not exist!")

			std::stringstream v;
		v << a.rdbuf();

		return Create(v.str());
	}

	Ref<Shader> Shader::Create(std::string shader)
	{
		switch (Platform::getAPI())
		{
		case Platform::API::None: CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return createRef<OpenGLShader>(shader);
		}
		CORE_ASSERT(false, "No Rendering API detected!")
			return nullptr;
	}

}
