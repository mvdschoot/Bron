//
// Created by mathijs on 8/1/25.
//

#pragma once
#include "Components/BufferExtentions.h"
#include "Shader.h"


#define PHONG_SHADER "phong_shader"
#define PBR_SHADER "pbr_shader"

namespace bron {

class ShaderRegistry {
public:
	static void Init();

    static Ref<Shader> GetShader(const char* name);
    static void CreateShader(const char* name, const char* pathToShader);

    // For shaders that are already in memory, such as the engine's built-in
    // ones (see BuiltinShaders.h).
    static void CreateShaderFromSource(const char* name, const std::string& source);

private:
    static std::map<std::string, Ref<Shader>> shaders;
};

} // Bron
