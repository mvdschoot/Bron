#ifndef __Renderer2Dh__
#define __Renderer2Dh__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"

#include "Command.h"
#include "../Camera.h"
#include "../Texture.h"
#include "../Shader.h"

#include "glm/glm.hpp"

#include <array>


namespace Steve::graphics
{
	class STEVE_API R2D
	{
	public:
		static void Init();

		static void BeginScene(Camera* camera);
		static void EndScene();

		static void NewBatch();
		static void NextBatch();

		/*
		 * 2D shaders work with a template for variables.
		 * The input is this:
		 *	
				{"a_Position", ShaderDataType::Float2},
				{"a_Color", ShaderDataType::Float4},
				{"a_TexCoord", ShaderDataType::Float2},
				{"a_TexIndex", ShaderDataType::Float}

		 * It also need uTextures and uVPmatrix as uniforms.
		 */
		static uint8_t AddShader(Ref<Shader> shader, BufferLayout buffer_layout);
		static void ActiveShader(uint8_t shader_number);
		static u32 GetActiveShader();

		static void DrawQuad(glm::vec3 pos, glm::vec3 dimension, glm::vec4 color);
		static void DrawQuad(glm::vec3 pos, glm::vec3 dimension, const Ref<Texture> texture);
		static void DrawQuad(glm::vec3 pos, glm::vec3 dimension, const Ref<Texture> texture, glm::vec4 texCoordsAndDims);

		static u32 GetTotQuadCount();
		static u32 GetTotQuadIndexCount();
		static u32 GetTotVertexCount();

	private:
		static void AddVAO(Ref<Shader> shader, BufferLayout buffer_layout);
		static void Flush();
	};
}


#endif // __Core/Renderer2D.h__
