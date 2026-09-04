#include "2D.h"

#include "Bron/Graphics/BuiltinShaders.h"


namespace bron
{
	struct QuadVertex
	{
		glm::vec2 pos;
		glm::vec4 color;
		glm::vec2 tex_coord;
		glm::vec1 tex_index;
	};

	// Because of multiple shaders
	struct VAO
	{
		Ref<VertexArray> quad_vertex_array;
		Ref<VertexBuffer> quad_buffer;
		Ref<Shader> shader;

		QuadVertex* quad_vertex_buffer_base;
		QuadVertex* quad_vertex_buffer_ptr;

		u32 quad_count = 0;
	};

	struct Renderer2DData
	{
		static constexpr u32 max_vertices = 1e6;
		static constexpr u32 max_indices = 1e6;
		static constexpr u32 max_tex_units = 32;

		u32 quad_index_count = 0;
		u32 quad_vertex_count = 0;

		// Texture stuff
		Ref<Texture> white_texture;
		std::array<Ref<Texture>, max_tex_units> texture_slots;
		u32 current_tex_slot = 1;

		Camera* camera;

		// Everything to graphics
		// to_render[0] == standard rendering
		std::vector<VAO> to_render;
		unsigned int render_state = 0;
	};

	static Renderer2DData s_data_2d;
#define CURRENT_RENDER_DATA s_data_2d.to_render[s_data_2d.render_state]

	void R2D::Init()
	{
		BR_PROFILE_FUNCTION();

		AddVAO(
			Shader::Create(builtin_shaders::Source(builtin_shaders::Id::kRenderer2D)), 
			BufferLayout({
				{"a_Position", ShaderDataType::kFloat2},
				{"a_Color", ShaderDataType::kFloat4},
				{"a_TexCoord", ShaderDataType::kFloat2},
				{"a_TexIndex", ShaderDataType::kFloat}
			})
		);

		s_data_2d.white_texture = Texture2D::Create(1, 1);
		u32 white_data = 0xffffffff;
		s_data_2d.white_texture->SetData(&white_data, sizeof(u32));
	}

	void R2D::BeginScene(Camera* camera)
	{
		BR_PROFILE_FUNCTION();
		s_data_2d.camera = camera;
		s_data_2d.quad_index_count = 0;
		s_data_2d.quad_vertex_count = 0;

		for (VAO &vao : s_data_2d.to_render)
			vao.quad_count = 0;

		NewBatch();
	}

	void R2D::EndScene()
	{
		BR_PROFILE_FUNCTION();
		Flush();
	}

	void R2D::NewBatch()
	{
		BR_PROFILE_FUNCTION();
		s_data_2d.current_tex_slot = 1;
		for (VAO& vao : s_data_2d.to_render) {
			vao.quad_vertex_buffer_ptr = vao.quad_vertex_buffer_base;
			vao.quad_count = 0;
		}
	}

	void R2D::NextBatch()
	{
		BR_PROFILE_FUNCTION();
		Flush();
		NewBatch();
	}

	uint8_t R2D::AddShader(Ref<Shader> shader, BufferLayout buffer_layout)
	{
		AddVAO(shader, buffer_layout);
		return s_data_2d.to_render.size() - 1;
	}

	void R2D::ActiveShader(uint8_t shader_number)
	{
		BR_CORE_ASSERT(shader_number < (s_data_2d.to_render.size()), "Invalid shader number");
		s_data_2d.render_state = shader_number;
		NextBatch();
	}

	u32 R2D::GetActiveShader()
	{
		return s_data_2d.render_state;
	}

	void R2D::DrawQuad(const glm::vec3 pos, const glm::vec3 dimension, const glm::vec4 color)
	{
		BR_PROFILE_FUNCTION();

		if (++CURRENT_RENDER_DATA.quad_count * 4 >= Renderer2DData::max_vertices)
		{
			NextBatch();
		}

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = color;
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(0.0f, 0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = color;
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(1.0f, 0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = color;
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(1.0f, 1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = color;
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(0.0f, 1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		s_data_2d.quad_vertex_count += 4;
		s_data_2d.quad_index_count += 6;
	}

	void R2D::DrawQuad(const glm::vec3 pos, const glm::vec3 dimension, const Ref<Texture> texture)
	{
		BR_PROFILE_FUNCTION();
		if (++CURRENT_RENDER_DATA.quad_count * 4 >= Renderer2DData::max_vertices)
		{
			NextBatch();
		}

		u32 tex_index = 0;
		for (int x = 1; x < s_data_2d.current_tex_slot; x++)
		{
			if (s_data_2d.texture_slots[x] != nullptr && (*texture.get()) == (*(s_data_2d.texture_slots[x])))
			{
				tex_index = x;
				break;
			}
		}
		if (tex_index == 0)
		{
			s_data_2d.texture_slots[s_data_2d.current_tex_slot] = texture;
			tex_index = s_data_2d.current_tex_slot;
			s_data_2d.current_tex_slot++;
		}

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(0.0f, 0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(1.0f, 0.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(1.0f, 1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(0.0f, 1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		s_data_2d.quad_vertex_count += 4;
		s_data_2d.quad_index_count += 6;
	}

	void R2D::DrawQuad(glm::vec3 pos, glm::vec3 dimension, const Ref<Texture> texture,
		glm::vec4 tex_coords_and_dims)
	{
		BR_PROFILE_FUNCTION();
		if (++CURRENT_RENDER_DATA.quad_count * 4 >= Renderer2DData::max_vertices)
		{
			NextBatch();
		}
		
		u32 tex_index = 0;
		for (int x = 1; x < s_data_2d.current_tex_slot; x++)
		{
			if (s_data_2d.texture_slots[x] != nullptr && (*texture.get()) == (*(s_data_2d.texture_slots[x])))
			{
				tex_index = x;
				break;
			}
		}

		if (tex_index == 0)
		{
			s_data_2d.texture_slots[s_data_2d.current_tex_slot] = texture;
			tex_index = s_data_2d.current_tex_slot;
			s_data_2d.current_tex_slot++;
		}

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(tex_coords_and_dims.x, tex_coords_and_dims.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(tex_coords_and_dims.x + tex_coords_and_dims.z, tex_coords_and_dims.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x + dimension.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(tex_coords_and_dims.x + tex_coords_and_dims.z, tex_coords_and_dims.y + tex_coords_and_dims.w);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->pos = glm::vec2(pos.x, pos.y + dimension.y);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->color = glm::vec4(1.0f);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_coord = glm::vec2(tex_coords_and_dims.x, tex_coords_and_dims.y + tex_coords_and_dims.w);
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr->tex_index = glm::vec1(static_cast<float>(tex_index));
		CURRENT_RENDER_DATA.quad_vertex_buffer_ptr++;

		s_data_2d.quad_vertex_count += 4;
		s_data_2d.quad_index_count += 6;
	}


	u32 R2D::GetTotQuadCount()
	{
		u32 res = 0;
		for (VAO& vao : s_data_2d.to_render)
			res += vao.quad_count;
		return res;
	}
	u32 R2D::GetTotQuadIndexCount()
	{
		return s_data_2d.quad_index_count;
	}

	u32 R2D::GetTotVertexCount()
	{
		return s_data_2d.quad_vertex_count;
	}

	void R2D::AddVAO(Ref<Shader> shader, BufferLayout buffer_layout)
	{
		BR_PROFILE_FUNCTION();
		VAO& vao = s_data_2d.to_render.emplace_back();

		vao.quad_vertex_array = VertexArray::Create();
		vao.quad_buffer = VertexBuffer::Create(s_data_2d.max_vertices * sizeof(QuadVertex));
		vao.quad_buffer->SetBufferLayout(buffer_layout);
		vao.quad_vertex_array->AddVertexBuffer(vao.quad_buffer);


		vao.quad_vertex_buffer_base = new QuadVertex[Renderer2DData::max_vertices];
		vao.quad_vertex_buffer_ptr = vao.quad_vertex_buffer_base;
		memset(vao.quad_vertex_buffer_base, 0, Renderer2DData::max_vertices);

		const auto quad_index_buffer = new u32[Renderer2DData::max_indices];
		int offset = 0;
		for (u32 x = 6; x < Renderer2DData::max_indices; x += 6)
		{
			quad_index_buffer[x - 6] = offset + 0;
			quad_index_buffer[x - 5] = offset + 1;
			quad_index_buffer[x - 4] = offset + 2;

			quad_index_buffer[x - 3] = offset + 2;
			quad_index_buffer[x - 2] = offset + 3;
			quad_index_buffer[x - 1] = offset + 0;
			offset += 4;
		}
		const Ref<IndexBuffer> IBuffer = IndexBuffer::Create(quad_index_buffer, Renderer2DData::max_indices);
		vao.quad_vertex_array->SetIndexBuffer(IBuffer);
		delete[] quad_index_buffer;

		vao.shader = shader;

		int arr[Renderer2DData::max_tex_units];
		for (int x = 0; x < Renderer2DData::max_tex_units; x++)
		{
			arr[x] = x;
		}

		vao.shader->Bind();
		vao.shader->SetUniform1iv("uTextures", arr, Renderer2DData::max_tex_units);

		s_data_2d.texture_slots[0] = s_data_2d.white_texture;
	}

	void R2D::Flush()
	{
		BR_PROFILE_FUNCTION();

		for (auto& [quad_vertex_array, quad_buffer, shader, quad_vertex_buffer_base, quad_vertex_buffer_ptr, quad_count] : s_data_2d.to_render) {

			// Quad graphics
			if (quad_count == 0)
				continue;
			quad_vertex_array->Bind();
			shader->Bind();
			for (int x = 0; x < s_data_2d.current_tex_slot; x++)
			{
				s_data_2d.texture_slots[x]->Bind(x);
			}

			const u32 size = static_cast<u32>((uint8_t*)quad_vertex_buffer_ptr - (uint8_t*)quad_vertex_buffer_base);
			quad_buffer->SetBufferData(quad_vertex_buffer_base, size);
			shader->SetUniformMat4("uVPmatrix", s_data_2d.camera->GetVPmatrix());
			Command::DrawIndexed(quad_vertex_array, quad_count * 6);
		}
	}
}
