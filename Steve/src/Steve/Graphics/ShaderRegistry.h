//
// Created by mathijs on 8/1/25.
//

#ifndef SHADERREGISTRY_H
#define SHADERREGISTRY_H
#include "Components/BufferExtentions.h"
#include "Shader.h"


#define PHONG_SHADER "phong_shader"
#define PBR_SHADER "pbr_shader"

namespace Steve {

class ShaderRegistry {
public:
	static void Init();

    static Ref<Shader> GetShader(const char* name);
    static void CreateShader(const char* name, const char* pathToShader);

private:
    static std::map<std::string, Ref<Shader>> shaders;
};

} // Steve

#endif //SHADERREGISTRY_H
