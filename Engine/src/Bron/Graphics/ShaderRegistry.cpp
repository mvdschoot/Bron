//
// Created by mathijs on 8/1/25.
//

#include "ShaderRegistry.h"

#include "BuiltinShaders.h"

namespace bron {
std::map<std::string, Ref<Shader>> ShaderRegistry::shaders_ = std::map<std::string, Ref<Shader>>();

void ShaderRegistry::Init() {
	ShaderRegistry::CreateShaderFromSource(BR_PHONG_SHADER, builtin_shaders::Source(builtin_shaders::Id::kPhong3D));
}

Ref<Shader> ShaderRegistry::GetShader(const char* name) {
	BR_CORE_ASSERT(shaders_.contains(name), "Shader by the name '{}', is not present in the shader registry", name);
	return shaders_[name];
}

void ShaderRegistry::CreateShader(const char* name, const char* path_to_shader) {
	const Ref<Shader> new_shader = Shader::CreateShaderFromLocation(path_to_shader);
	shaders_[name] = new_shader;
}

void ShaderRegistry::CreateShaderFromSource(const char* name, const std::string& source) {
	shaders_[name] = Shader::Create(source);
}
} // namespace bron
