//
// Created by mathijs on 8/2/25.
//

#pragma once

#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/Components/BufferExtentions.h"
#include "Bron/Graphics/ShaderDataFunctions.h"
#include "Bron/Scene/Components.h"

namespace bron {

	inline const auto PhongVertexLayout = NamedBufferLayout<VertexVariables>({
			{VertexVariables::kPositions, {"a_Position", ShaderDataType::kFloat3}},
			{VertexVariables::kNormals, {"a_Normal", ShaderDataType::kFloat3}},
			{VertexVariables::kUvs, {"a_TexCoords", ShaderDataType::kFloat2}}
	});
}
