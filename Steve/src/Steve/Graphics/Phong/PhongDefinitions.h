//
// Created by mathijs on 8/2/25.
//

#ifndef PHONGVERTEX_H
#define PHONGVERTEX_H

#include "Steve/Graphics/Buffer.h"
#include "Steve/Graphics/Components/BufferExtentions.h"
#include "Steve/Graphics/ShaderDataFunctions.h"
#include "Steve/Scene/Components.h"

namespace Steve {

	inline const auto PhongVertexLayout = NamedBufferLayout<VertexVariables>({
			{VertexVariables::POSITIONS, {"a_Position", ShaderDataType::Float3}},
			{VertexVariables::NORMALS, {"a_Normal", ShaderDataType::Float3}},
			{VertexVariables::UVS, {"a_TexCoords", ShaderDataType::Float2}}
	});
}

#endif //PHONGVERTEX_H
