#ifndef __OPENGLSHADER_H__
#define __OPENGLSHADER_H__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Steve/Graphics/Shader.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <set>
#include <regex>
#include <fstream>

namespace Steve::graphics
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(std::string& shader);
		~OpenGLShader();

		void bind() override;
		void unbind() override;
		bool isBound() override;
		bool isSet(std::string name) override;

		void setUniformMat3(std::string name, const glm::mat3& matrix) override;
		void setUniformMat4(std::string name, const glm::mat4& matrix) override;
		void setUniform1i(std::string name, u32 a) override;
		void setUniform2i(std::string name, u32 a, u32 b) override;
		void setUniform3i(std::string name, u32 a, u32 b, u32 c) override;
		void setUniform4i(std::string name, u32 a, u32 b, u32 c, u32 d) override;
		void setUniform1f(std::string name, float a) override;
		void setUniform2f(std::string name, float a, float b) override;
		void setUniform3f(std::string name, float a, float b, float c) override;
		void setUniform4f(std::string name, float a, float b, float c, float d) override;

		void setUniform1iv(std::string name, int* array, int count) override;

	private:
		void compile();
		void parse(std::string& shader);

		std::string _v_shader, _g_shader, _f_shader;
		u32 _renderer_id;

		bool _binded;
		std::unordered_map<std::string, u32> _uniform_locations;
	};
}

#endif // __OPENGLSHADER_H__
