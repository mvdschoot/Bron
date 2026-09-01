//
// Created by mathijs on 8/1/25.
//

#include "ShaderRegistry.h"

#include "Steve/Util/Paths.h"

namespace Steve {
	std::map<std::string, Ref<Shader>> ShaderRegistry::shaders = std::map<std::string, Ref<Shader>>();

	void ShaderRegistry::Init() {
		ShaderRegistry::CreateShader(PHONG_SHADER, Paths::EngineAssetString("Phong3DShader.glsl").c_str());
	}

	Ref<Shader> ShaderRegistry::GetShader(const char *name) {
        CORE_ASSERT(shaders.contains(name), "Shader by the name '{}', is not present in the shader registry", name);
        return shaders[name];
    }

    void ShaderRegistry::CreateShader(const char *name, const char *pathToShader) {
        const Ref<Shader> newShader = Shader::CreateShaderFromLocation(pathToShader);
        shaders[name] = newShader;
    }
} // Steve