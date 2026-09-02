//
// Created by mathijs on 8/2/25.
//

#pragma once

#include "Bron/Graphics/Buffer.h"
#include "Bron/Graphics/Components/BufferExtentions.h"
#include "Bron/Graphics/ShaderDataFunctions.h"
#include "Bron/Scene/Components.h"

namespace Bron {

	inline const auto PhongVertexLayout = NamedBufferLayout<VertexVariables>({
			{VertexVariables::POSITIONS, {"a_Position", ShaderDataType::Float3}},
			{VertexVariables::NORMALS, {"a_Normal", ShaderDataType::Float3}},
			{VertexVariables::UVS, {"a_TexCoords", ShaderDataType::Float2}}
	});
}
