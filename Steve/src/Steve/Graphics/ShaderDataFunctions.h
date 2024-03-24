#ifndef _SHADERDATAFUNCTIONS_HEADER__
#define _SHADERDATAFUNCTIONS_HEADER__

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Steve
{
	enum class STEVE_API ShaderDataType : u32
	{
		None = 0,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat2,
		Mat3,
		Mat4,
		Bool
	};

	static u32 ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int: return 4;
		case ShaderDataType::Int2: return 4 * 2;
		case ShaderDataType::Int3: return 4 * 3;
		case ShaderDataType::Int4: return 4 * 4;
		case ShaderDataType::Float: return 4;
		case ShaderDataType::Float2: return 4 * 2;
		case ShaderDataType::Float3: return 4 * 3;
		case ShaderDataType::Float4: return 4 * 4;
		case ShaderDataType::Mat2: return 4 * 2 * 2;
		case ShaderDataType::Mat3: return 4 * 3 * 3;
		case ShaderDataType::Mat4: return 4 * 4 * 4;
		case ShaderDataType::Bool: return 1;
		}

		CORE_ASSERT(false, "Invalid shader data type")
	}

	static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int: return GL_INT;
		case ShaderDataType::Int2: return GL_INT;
		case ShaderDataType::Int3: return GL_INT;
		case ShaderDataType::Int4: return GL_INT;
		case ShaderDataType::Float: return GL_FLOAT;
		case ShaderDataType::Float2: return GL_FLOAT;
		case ShaderDataType::Float3: return GL_FLOAT;
		case ShaderDataType::Float4: return GL_FLOAT;
		case ShaderDataType::Mat2: return GL_FLOAT;
		case ShaderDataType::Mat3: return GL_FLOAT;
		case ShaderDataType::Mat4: return GL_FLOAT;
		case ShaderDataType::Bool: return GL_BOOL;
		}

		CORE_ASSERT(false, "Invalid Shader data type!")
		return 0;
	}

	static u32 ShaderDataTypeElementCount(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Int: return 1;
		case ShaderDataType::Int2: return 2;
		case ShaderDataType::Int3: return 3;
		case ShaderDataType::Int4: return 4;
		case ShaderDataType::Float: return 1;
		case ShaderDataType::Float2: return 2;
		case ShaderDataType::Float3: return 3;
		case ShaderDataType::Float4: return 4;
		case ShaderDataType::Mat2: return 4;
		case ShaderDataType::Mat3: return 9;
		case ShaderDataType::Mat4: return 16;
		case ShaderDataType::Bool: return 1;
		}

		CORE_ASSERT(false, "Invalid Shader data type!")
		return -1;
	}
}

#endif