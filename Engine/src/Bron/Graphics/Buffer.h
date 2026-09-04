#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

#include "ShaderDataFunctions.h"

#include <string>
#include <vector>


namespace bron {

struct BR_API BufferElement {
	std::string name;
	ShaderDataType type;
	u32 size;
	u32 offset;
	bool normalized;

	BufferElement() = default;

	BufferElement(std::string name, ShaderDataType type) :
		name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(false) {}

	~BufferElement() = default;

	u32 GetComponentCount() const {
		switch (type) {
			case ShaderDataType::kFloat:
				return 1;
			case ShaderDataType::kFloat2:
				return 2;
			case ShaderDataType::kFloat3:
				return 3;
			case ShaderDataType::kFloat4:
				return 4;
			case ShaderDataType::kMat3:
				return 3; // 3* float3
			case ShaderDataType::kMat4:
				return 4; // 4* float4
			case ShaderDataType::kInt:
				return 1;
			case ShaderDataType::kInt2:
				return 2;
			case ShaderDataType::kInt3:
				return 3;
			case ShaderDataType::kInt4:
				return 4;
			case ShaderDataType::kBool:
				return 1;
		}

		return 0;
	}
};

class BR_API BufferLayout {
public:
	BufferLayout() : stride_(0) {};

	BufferLayout(std::initializer_list<BufferElement> elements) : stride_(0), buffer_elements_(elements) {
		CalculateInfo();
	}

	//~_BufferLayout() = default;

	const std::vector<BufferElement>& GetElements() const { return buffer_elements_; }
	u32 GetStride() const { return stride_; }

	void CalculateInfo() {
		u32 offset = 0;
		stride_ = 0;
		for (auto& it: buffer_elements_) {
			it.offset = offset;
			offset += it.size;
			stride_ += it.size;
		}
	}

protected:
	void SetElements(std::vector<BufferElement>&& buffer) {
		buffer_elements_ = buffer;
		CalculateInfo();
	}


	u32 stride_;
	std::vector<BufferElement> buffer_elements_;
};

class BR_API VertexBuffer {
public:
	VertexBuffer() {};

	virtual ~VertexBuffer() {};

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual const BufferLayout& GetBufferLayout() const = 0;
	virtual void SetBufferLayout(const BufferLayout& layout) = 0;
	virtual void SetBufferData(const void* data, usize size) = 0;
	virtual void ResizeAndSetBuffer(const void* data, usize size) = 0;

	static Ref<VertexBuffer> Create(usize size);
	static Ref<VertexBuffer> Create(float* vertices, usize size);
};


class BR_API IndexBuffer {
public:
	IndexBuffer() {};

	virtual ~IndexBuffer() {};

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	[[nodiscard]] virtual u32 GetCount() const = 0;
	virtual void SetBufferData(u32* data, u32 count) = 0;
	virtual void ResizeAndSetBuffer(u32* data, u32 count) = 0;

	static Ref<IndexBuffer> Create(u32* indices, u32 count);
	static Ref<IndexBuffer> Create(usize size);
};

class BR_API UniformBuffer {
public:
	UniformBuffer() = default;
	virtual ~UniformBuffer() = default;

	virtual void Bind(u32 binding) = 0;
	virtual void Unbind() = 0;
	virtual bool IsBound() = 0;

	virtual void SetData(const uint8_t* data, uint64_t size, uint64_t offset = 0) = 0;
	virtual void ResizeAndSetData(const uint8_t* data, uint64_t size) = 0;

	// Factory methods
	static Ref<UniformBuffer> Create(usize size, u32 binding);
	static Ref<UniformBuffer> Create(const void* data, usize size, u32 binding);
};
} // namespace bron
