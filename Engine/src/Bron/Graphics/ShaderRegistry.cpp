//
// Created by mathijs on 8/1/25.
//

#include "ShaderRegistry.h"

#include "BuiltinShaders.h"

namespace Bron {
	std::map<std::string, Ref<Shader>> ShaderRegistry::shaders = std::map<std::string, Ref<Shader>>();

	void ShaderRegistry::Init() {
		ShaderRegistry::CreateShaderFromSource(PHONG_SHADER, BuiltinShaders::Source(BuiltinShaders::Id::Phong3D));
	}

	Ref<Shader> ShaderRegistry::GetShader(const char *name) {
        CORE_ASSERT(shaders.contains(name), "Shader by the name '{}', is not present in the shader registry", name);
        return shaders[name];
    }

    void ShaderRegistry::CreateShader(const char *name, const char *pathToShader) {
        const Ref<Shader> newShader = Shader::CreateShaderFromLocation(pathToShader);
        shaders[name] = newShader;
    }

    void ShaderRegistry::CreateShaderFromSource(const char *name, const std::string &source) {
        shaders[name] = Shader::Create(source);
    }
} // Bron