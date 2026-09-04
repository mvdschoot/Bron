#include "OpenGLShader.h"

namespace bron
{
	void OpenGLShader::Bind()
	{
		glUseProgram(renderer_id_);
		binded_ = true;
	}

	void OpenGLShader::Unbind()
	{
		glUseProgram(0);
		binded_ = false;
	}

	bool OpenGLShader::IsBound()
	{
		return binded_;
	}

	bool OpenGLShader::IsSet(std::string name)
	{
		return uniform_locations_.find(name) != uniform_locations_.end();
	}


	OpenGLShader::OpenGLShader(std::string& shader)
	{
		parse(shader);
		Compile();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(renderer_id_);
	}

	void OpenGLShader::parse(std::string& shader)
	{
		BR_PROFILE_FUNCTION();

		const std::string v_word = "#type vertex\n";
		const std::string f_word = "#type fragment\n";
		const std::string g_word = "#type geometry\n";
		usize v = shader.find(v_word);
		usize f = shader.find(f_word);
		usize g = shader.find(g_word);

		BR_CORE_ASSERT(v != -1 && f != -1, "Both vertex and fragment shader should be present.");

		if (g == -1)
		{
			v_shader_ = shader.substr(v + v_word.size(), f - v_word.size());
			f_shader_ = shader.substr(f + f_word.size(), std::string::npos);
		}
		else
		{
			v_shader_ = shader.substr(v + v_word.size(), g - v_word.size());
			g_shader_ = shader.substr(g + g_word.size(), f - g - g_word.size());
			f_shader_ = shader.substr(f + f_word.size(), std::string::npos);
		}
	}

	void OpenGLShader::Compile()
	{
		BR_PROFILE_FUNCTION();

		// Vertex shader
		const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

		auto source = v_shader_.c_str();
		glShaderSource(vertex_shader, 1, &source, nullptr);

		glCompileShader(vertex_shader);
		GLint is_compiled = 0;
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(vertex_shader, max_length, &max_length, &info_log[0]);


			glDeleteShader(vertex_shader);

			BR_CORE_ERROR("Failed to compile vertex shader: {}", info_log.data());

			return;
		}


		// Fragment shader
		const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		source = f_shader_.c_str();
		glShaderSource(fragment_shader, 1, &source, nullptr);

		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == GL_FALSE)
		{
			GLint max_length = 0;
			glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetShaderInfoLog(fragment_shader, max_length, &max_length, &info_log[0]);


			glDeleteShader(fragment_shader);
			glDeleteShader(vertex_shader);

			BR_CORE_ERROR("Failed to compile fragment shader: {}", info_log.data());

			return;
		}


		// (Optional) geometry shader
		GLuint geometry_shader = -1;
		if (!g_shader_.empty())
		{
			geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);

			source = g_shader_.c_str();
			glShaderSource(geometry_shader, 1, &source, nullptr);

			glCompileShader(geometry_shader);
			glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &is_compiled);
			if (is_compiled == GL_FALSE)
			{
				GLint max_length = 0;
				glGetShaderiv(geometry_shader, GL_INFO_LOG_LENGTH, &max_length);

				std::vector<GLchar> info_log(max_length);
				glGetShaderInfoLog(geometry_shader, max_length, &max_length, &info_log[0]);

				glDeleteShader(geometry_shader);
				glDeleteShader(fragment_shader);
				glDeleteShader(vertex_shader);

				BR_CORE_ERROR("Failed to compile geometry shader: {}", info_log.data());

				return;
			}
		}


		renderer_id_ = glCreateProgram();

		glAttachShader(renderer_id_, vertex_shader);
		glAttachShader(renderer_id_, fragment_shader);
		if (!g_shader_.empty())
			glAttachShader(renderer_id_, geometry_shader);


		glLinkProgram(renderer_id_);

		GLint is_linked = 0;
		glGetProgramiv(renderer_id_, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint max_length = 0;
			glGetProgramiv(renderer_id_, GL_INFO_LOG_LENGTH, &max_length);

			std::vector<GLchar> info_log(max_length);
			glGetProgramInfoLog(renderer_id_, max_length, &max_length, &info_log[0]);


			glDeleteProgram(renderer_id_);

			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);
			glDeleteShader(geometry_shader);

			BR_CORE_ERROR("Failed to link shaders into renderer_id_: {}", info_log.data());


			return;
		}

		BR_CORE_INFO("Loaded shader {} into program!", renderer_id_);

		glDetachShader(renderer_id_, vertex_shader);
		glDetachShader(renderer_id_, fragment_shader);

		if (!g_shader_.empty())
			glDetachShader(renderer_id_, geometry_shader);
	}

	void OpenGLShader::SetUniformMat3(std::string name, const glm::mat3& matrix)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniformMatrix3fv(loc, 1, GL_FALSE, value_ptr(matrix));
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniformMat4(std::string name, const glm::mat4& matrix)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(matrix));
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform1i(std::string name, u32 a)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform1i(loc, a);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform2i(std::string name, u32 a, u32 b)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform2i(loc, a, b);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform3i(std::string name, u32 a, u32 b, u32 c)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform3i(loc, a, b, c);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform4i(std::string name, u32 a, u32 b, u32 c, u32 d)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform4i(loc, a, b, c, d);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform1f(std::string name, float a)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform1f(loc, a);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform2f(std::string name, float a, float b)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform2f(loc, a, b);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform3f(std::string name, float a, float b, float c)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform3f(loc, a, b, c);
		uniform_locations_[name] = loc;
	}

	void OpenGLShader::SetUniform4f(std::string name, float a, float b, float c, float d)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform4f(loc, a, b, c, d);
		uniform_locations_.emplace(name, loc);
	}

	void OpenGLShader::SetUniform1iv(std::string name, int* array, int count)
	{
		u32 loc = uniform_locations_.contains(name) ? uniform_locations_[name] : glGetUniformLocation(renderer_id_, name.c_str());
		BR_CORE_ASSERT(loc != -1, "Cant find uniform ({}) or uniform not used in shader", name);
		glUniform1iv(loc, count, array);
		uniform_locations_.emplace(name, loc);
	}
}
