//
// Created by mathijs on 8/1/25.
//

#pragma once

#include "Components/BufferExtentions.h"
#include "Shader.h"

namespace Bron {

	enum MaterialWorkflow {
		PHONG,
		// PBR,
	};

	class MaterialBase {
	public:
		MaterialWorkflow workflow;
		const char* shaderName;

		MaterialBase(const MaterialWorkflow wf, const char* sh)
			: workflow(wf), shaderName(sh) {}

		virtual ~MaterialBase() = default;

		// Bind material data to the shader.
		virtual u8 Bind(Ref<Shader> shader, u8 textureSlotStartingIndex) = 0;

		// For statistics, gets the number of uniform calls.
		virtual u32 NumberUniformCalls() const = 0;
	};

}
