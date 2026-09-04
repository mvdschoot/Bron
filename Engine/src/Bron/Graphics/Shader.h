#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"
#include "Bron/Core/Profiling.h"

#include "Buffer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace bron
{
	class Shader
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual bool IsBound() = 0;
		virtual bool IsSet(std::string name) = 0;
		virtual void SetUniformMat3(std::string name, const glm::mat3& matrix) = 0;
		virtual void SetUniformMat4(std::string name, const glm::mat4& matrix) = 0;
		virtual void SetUniform1i(std::string name, u32 a) = 0;
		virtual void SetUniform2i(std::string name, u32 a, u32 b) = 0;
		virtual void SetUniform3i(std::string name, u32 a, u32 b, u32 c) = 0;
		virtual void SetUniform4i(std::string name, u32 a, u32 b, u32 c, u32 d) = 0;
		virtual void SetUniform1f(std::string name, float a) = 0;
		virtual void SetUniform2f(std::string name, float a, float b) = 0;
		virtual void SetUniform3f(std::string name, float a, float b, float c) = 0;
		virtual void SetUniform4f(std::string name, float a, float b, float c, float d) = 0;

		virtual void SetUniform1iv(std::string name, int* array, int count) = 0;

		void SetUniform(ShaderDataType type, std::string name, u8* data);
		void SetUniforms(const BufferLayout& layout, u8* data);

		static Ref<Shader> Create(std::string shader);
		static Ref<Shader> CreateShaderFromLocation(const std::string& shader_loc);
	};

	inline void Shader::SetUniform(ShaderDataType type, std::string name, u8* data)
	{
		if (type == ShaderDataType::Float) return SetUniform1f(std::move(name), *(float*)data);
		if (type == ShaderDataType::Float2) return SetUniform2f(std::move(name), *(float*) data, *((float*)data + 1));
		if (type == ShaderDataType::Float3) return SetUniform3f(std::move(name), *((float*)data), *((float*)data + 1), *((float*)data + 2));
		if (type == ShaderDataType::Float4) return SetUniform4f(std::move(name), *((float*)data), *((float*)data + 1), *((float*)data + 2), *((float*)data + 3));
		if (type == ShaderDataType::Int) return SetUniform1i(std::move(name), *((u32*)data));
		if (type == ShaderDataType::Int2) return SetUniform2i(std::move(name), *((u32*)data), *((u32*)data + 1));
		if (type == ShaderDataType::Int3) return SetUniform3i(std::move(name), *((u32*)data), *((u32*)data + 1), *((u32*)data + 2));
		if (type == ShaderDataType::Int4) return SetUniform4i(std::move(name), *((u32*)data), *((u32*)data + 1), *((u32*)data + 2), *((u32*)data + 3));
		if (type == ShaderDataType::Mat3) return SetUniformMat3(std::move(name), *(glm::mat3*)data);
		if (type == ShaderDataType::Mat4) return SetUniformMat4(std::move(name), *(glm::mat4*)data);

		BR_CORE_ASSERT(false, "Uniform type not supported");
	}

	inline void Shader::SetUniforms(const BufferLayout& layout, u8* data)
	{
		const std::vector<BufferElement>& elements = layout.GetElements();
		for (const BufferElement& el : elements)
		{
			SetUniform(el.type, el.name, data + el.offset);
		}
	}
}
