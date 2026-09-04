#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Bron/Core/Platform.h"

namespace bron
{
	Ref<Shader> Shader::CreateShaderFromLocation(const std::string& shader_loc)
	{
		std::ifstream a(shader_loc);
		BR_CORE_ASSERT(!a.fail(), "Shaderloc does not exist!");

		std::stringstream v;
		v << a.rdbuf();

		return Create(v.str());
	}

	Ref<Shader> Shader::Create(std::string shader)
	{
		switch (Platform::GetApi())
		{
		case Platform::API::None: BR_CORE_ASSERT(false, "No Rendering API selected!");
		case Platform::API::OpenGL: return CreateRef<OpenGLShader>(shader);
		}
		BR_CORE_ASSERT(false, "No Rendering API detected!");
			return nullptr;
	}

}
