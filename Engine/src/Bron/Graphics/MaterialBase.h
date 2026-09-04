//
// Created by mathijs on 8/1/25.
//

#pragma once

#include "Components/BufferExtentions.h"
#include "Shader.h"

namespace bron {

	enum MaterialWorkflow {
		kPhong,
		// PBR,
	};

	class MaterialBase {
	public:
		MaterialWorkflow workflow;
		const char* shader_name;

		MaterialBase(const MaterialWorkflow wf, const char* sh)
			: workflow(wf), shader_name(sh) {}

		virtual ~MaterialBase() = default;

		// Bind material data to the shader.
		virtual u8 Bind(Ref<Shader> shader, u8 texture_slot_starting_index) = 0;

		// For statistics, gets the number of uniform calls.
		virtual u32 NumberUniformCalls() const = 0;
	};

}
