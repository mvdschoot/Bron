#include "OpenGLShader.h"

namespace Steve
{
	void OpenGLShader::bind()
	{
		glUseProgram(_renderer_id);
		_binded = true;
	}

	void OpenGLShader::unbind()
	{
		glUseProgram(0);
		_binded = false;
	}

	bool OpenGLShader::isBound()
	{
		return _binded;
	}

	bool OpenGLShader::isSet(std::string name)
	{
		return _uniform_locations.find(name) != _uniform_locations.end();
	}


	OpenGLShader::OpenGLShader(std::string& shader)
	{
		parse(shader);
		compile();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(_renderer_id);
	}

	void OpenGLShader::parse(std::string& shader)
	{
		CH_PROFILE_FUNCTION();

		const std::string v_word = "#type vertex\n";
		const std::string f_word = "#type fragment\n";
		const std::string g_word = "#type geometry\n";
		int v = shader.find(v_word);
		int f = shader.find(f_word);
		int g = shader.find(g_word);

		CORE_ASSERT(v != -1 && f != -1, "Both vertex and fragment shader should be present.")

		if (g == -1)
		{
			_v_shader = shader.substr(v + v_word.size(), f - v_word.size());
			_f_shader = shader.substr(f + f_word.size(), shader.npos);
		}
		else
		{
			_v_shader = shader.substr(v + v_word.size(), g - v_word.size());
			_g_shader = shader.substr(g + g_word.size(), f - g - g_word.size());
			_f_shader = shader.substr(f + f_word.size(), shader.npos);
		}
	}

	void OpenGLShader::compile()
	{
		CH_PROFILE_FUNCTION();

		// Vertex shader
		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		auto source = _v_shader.c_str();
		glShaderSource(vertexShader, 1, &source, nullptr);

		glCompileShader(vertexShader);
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);


			glDeleteShader(vertexShader);

			CORE_ERROR("Failed to compile vertex shader: {}", infoLog.data());

			return;
		}


		// Fragment shader
		const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = _f_shader.c_str();
		glShaderSource(fragmentShader, 1, &source, nullptr);

		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);


			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			CORE_ERROR("Failed to compile fragment shader: {}", infoLog.data());

			return;
		}


		// (Optional) geometry shader
		GLuint geometryShader = -1;
		if (!_g_shader.empty())
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			source = _g_shader.c_str();
			glShaderSource(geometryShader, 1, &source, nullptr);

			glCompileShader(geometryShader);
			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(geometryShader);
				glDeleteShader(fragmentShader);
				glDeleteShader(vertexShader);

				CORE_ERROR("Failed to compile geometry shader: {}", infoLog.data());

				return;
			}
		}


		_renderer_id = glCreateProgram();

		glAttachShader(_renderer_id, vertexShader);
		glAttachShader(_renderer_id, fragmentShader);
		if (!_g_shader.empty())
			glAttachShader(_renderer_id, geometryShader);


		glLinkProgram(_renderer_id);

		GLint isLinked = 0;
		glGetProgramiv(_renderer_id, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_renderer_id, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(_renderer_id, maxLength, &maxLength, &infoLog[0]);


			glDeleteProgram(_renderer_id);

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteShader(geometryShader);

			CORE_ERROR("Failed to link shaders into _renderer_id: {}", infoLog.data());


			return;
		}

		CORE_INFO("Loaded shader {} into program!", _renderer_id);

		glDetachShader(_renderer_id, vertexShader);
		glDetachShader(_renderer_id, fragmentShader);

		if (!_g_shader.empty())
			glDetachShader(_renderer_id, geometryShader);
	}

	void OpenGLShader::setUniformMat3(std::string name, const glm::mat3& matrix)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(matrix));
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniformMat4(std::string name, const glm::mat4& matrix)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(matrix));
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform1i(std::string name, u32 a)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform1i(loc, a);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform2i(std::string name, u32 a, u32 b)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform2i(loc, a, b);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform3i(std::string name, u32 a, u32 b, u32 c)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform3i(loc, a, b, c);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform4i(std::string name, u32 a, u32 b, u32 c, u32 d)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform4i(loc, a, b, c, d);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform1f(std::string name, float a)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform1f(loc, a);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform2f(std::string name, float a, float b)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform2f(loc, a, b);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform3f(std::string name, float a, float b, float c)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform3f(loc, a, b, c);
		_uniform_locations[name] = loc;
	}

	void OpenGLShader::setUniform4f(std::string name, float a, float b, float c, float d)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform4f(loc, a, b, c, d);
		_uniform_locations.emplace(name, loc);
	}

	void OpenGLShader::setUniform1iv(std::string name, int* array, int count)
	{
		u32 loc = _uniform_locations.contains(name) ? _uniform_locations[name] : glGetUniformLocation(_renderer_id, name.c_str());
		CORE_ASSERT(loc != -1, "Cant find uniform or uniform not used in shader")
		glUniform1iv(loc, count, array);
		_uniform_locations.emplace(name, loc);
	}
}
