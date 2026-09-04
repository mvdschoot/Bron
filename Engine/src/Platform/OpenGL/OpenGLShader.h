#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "Bron/Graphics/Shader.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <set>
#include <regex>
#include <fstream>

namespace bron {
class OpenGLShader : public Shader {
public:
	OpenGLShader(std::string& shader);
	~OpenGLShader();

	void Bind() override;
	void Unbind() override;
	bool IsBound() override;
	bool IsSet(std::string name) override;

	void SetUniformMat3(std::string name, const glm::mat3& matrix) override;
	void SetUniformMat4(std::string name, const glm::mat4& matrix) override;
	void SetUniform1i(std::string name, u32 a) override;
	void SetUniform2i(std::string name, u32 a, u32 b) override;
	void SetUniform3i(std::string name, u32 a, u32 b, u32 c) override;
	void SetUniform4i(std::string name, u32 a, u32 b, u32 c, u32 d) override;
	void SetUniform1f(std::string name, float a) override;
	void SetUniform2f(std::string name, float a, float b) override;
	void SetUniform3f(std::string name, float a, float b, float c) override;
	void SetUniform4f(std::string name, float a, float b, float c, float d) override;

	void SetUniform1iv(std::string name, int* array, int count) override;

private:
	void Compile();
	void parse(std::string& shader);

	std::string v_shader_, g_shader_, f_shader_;
	u32 renderer_id_;

	bool binded_;
	std::unordered_map<std::string, u32> uniform_locations_;
};
} // namespace bron
