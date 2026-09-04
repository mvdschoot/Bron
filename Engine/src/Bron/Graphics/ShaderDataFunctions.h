#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace bron {
enum class BR_API ShaderDataType : u32 {
	kNone = 0,
	kInt,
	kInt2,
	kInt3,
	kInt4,
	kFloat,
	kFloat2,
	kFloat3,
	kFloat4,
	kMat2,
	kMat3,
	kMat4,
	kBool
};

static u32 ShaderDataTypeSize(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::kInt:
			return 4;
		case ShaderDataType::kInt2:
			return 4 * 2;
		case ShaderDataType::kInt3:
			return 4 * 3;
		case ShaderDataType::kInt4:
			return 4 * 4;
		case ShaderDataType::kFloat:
			return 4;
		case ShaderDataType::kFloat2:
			return 4 * 2;
		case ShaderDataType::kFloat3:
			return 4 * 3;
		case ShaderDataType::kFloat4:
			return 4 * 4;
		case ShaderDataType::kMat2:
			return 4 * 2 * 2;
		case ShaderDataType::kMat3:
			return 4 * 3 * 3;
		case ShaderDataType::kMat4:
			return 4 * 4 * 4;
		case ShaderDataType::kBool:
			return 1;
	}

	BR_CORE_ASSERT(false, "Invalid shader data type");
}

static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::kInt:
			return GL_INT;
		case ShaderDataType::kInt2:
			return GL_INT;
		case ShaderDataType::kInt3:
			return GL_INT;
		case ShaderDataType::kInt4:
			return GL_INT;
		case ShaderDataType::kFloat:
			return GL_FLOAT;
		case ShaderDataType::kFloat2:
			return GL_FLOAT;
		case ShaderDataType::kFloat3:
			return GL_FLOAT;
		case ShaderDataType::kFloat4:
			return GL_FLOAT;
		case ShaderDataType::kMat2:
			return GL_FLOAT;
		case ShaderDataType::kMat3:
			return GL_FLOAT;
		case ShaderDataType::kMat4:
			return GL_FLOAT;
		case ShaderDataType::kBool:
			return GL_BOOL;
	}

	BR_CORE_ASSERT(false, "Invalid Shader data type!");
	return 0;
}

static u32 ShaderDataTypeElementCount(ShaderDataType type) {
	switch (type) {
		case ShaderDataType::kInt:
			return 1;
		case ShaderDataType::kInt2:
			return 2;
		case ShaderDataType::kInt3:
			return 3;
		case ShaderDataType::kInt4:
			return 4;
		case ShaderDataType::kFloat:
			return 1;
		case ShaderDataType::kFloat2:
			return 2;
		case ShaderDataType::kFloat3:
			return 3;
		case ShaderDataType::kFloat4:
			return 4;
		case ShaderDataType::kMat2:
			return 4;
		case ShaderDataType::kMat3:
			return 9;
		case ShaderDataType::kMat4:
			return 16;
		case ShaderDataType::kBool:
			return 1;
	}

	BR_CORE_ASSERT(false, "Invalid Shader data type!");
	return -1;
}
} // namespace bron
