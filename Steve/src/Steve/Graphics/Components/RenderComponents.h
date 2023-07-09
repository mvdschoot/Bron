#ifndef __RENDERCOMPONENTS_HEADER__
#define __RENDERCOMPONENTS_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"

#include "Steve/Graphics/Texture.h"
#include "Steve/Graphics/Buffer.h"
#include "Steve/Graphics/VertexArray.h"
#include "Steve/Graphics/Shader.h"

#include <map>

#include "MiscellaneousComponents.h"
#include "Steve/ECS/Entity.h"
#include "Steve/Graphics/Camera.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"


namespace Steve::graphics
{


	enum class TextureType
	{
		DIFFUSE,
		SPECULAR,
		NORMAL
	};

	enum class ColorType
	{
		AmbientFactor,
		Diffuse,
		Specular,
		Shininess,
		ShininessStrength
	};

	enum class LightData
	{
		Position,
		Color
	};

	inline usize ColorTypeSize(ColorType type) {
		switch (type)
		{
		case ColorType::Shininess:
		case ColorType::ShininessStrength:
			return sizeof(float);
		default:
			return sizeof(glm::vec3);
		}
	}

	struct TexturePack
	{
		std::map<TextureType, Ref<Texture>> Textures;
	};

	/**
	 * Keep in mind that the elements in the buffer are ColorElements + TextureElements.
	 * Also in that order.
	 */
	struct MaterialLayout : public BufferLayout
	{
		MaterialLayout(std::initializer_list<std::pair<ColorType, BufferElement>> colors, 
						std::initializer_list<std::pair<TextureType, BufferElement>> textures)
			: Textures({}), Colors({})
		{
			std::vector<BufferElement> b;
			for (std::pair<ColorType, BufferElement> el : colors) {
				Colors.push_back(el.first);
				b.push_back(el.second);
			}
			for (std::pair<TextureType, BufferElement> el : textures) {
				Textures.push_back(el.first);
				b.push_back(el.second);
			}

			SetElements(std::move(b));
		}

		[[nodiscard]] const BufferElement& GetElementData(ColorType colorType) const;
		[[nodiscard]] const BufferElement& GetElementData(TextureType textureType) const;

		std::vector<TextureType> Textures;
		std::vector<ColorType> Colors;
	};

	struct LightLayout : public BufferLayout
	{
		LightLayout(std::initializer_list<std::pair<LightData, BufferElement>> data)
			: Data({})
		{
			std::vector<BufferElement> b;
			for (std::pair<LightData, BufferElement> el : data) {
				Data.push_back(el.first);
				b.push_back(el.second);
			}

			SetElements(std::move(b));
		}

		void SetName(LightData data_type, std::string new_name);
		[[nodiscard]] const BufferElement& GetElementData(LightData light_data_type) const;

		std::vector<LightData> Data;
	};

	struct Material
	{
		const MaterialLayout* Layout;
		u8* Data;
		TexturePack* Textures;

		void Set(ColorType type, const uint8_t* value);
		void Set(TextureType type, int value);
	};

	struct LightContext
	{
		const LightLayout* Layout;
		u8* Data;

		void Set(LightData type, u8* value);
	};

	class MeshContext
	{
	public:
		MeshContext(Shader* shader, Material* material)
			: pMaterial(material), pShader(shader) {}

		~MeshContext() = default;

		Material* pMaterial;
		Shader* pShader;
	};

	/*
	 * One context per render component.
	 * The attributes of the context can be shared across all components..
	 */
	class Mesh
	{
	public:
		Mesh(const Mesh& mesh)
			: pContext(mesh.pContext), pVao(mesh.pVao), mBufferLayout(mesh.mBufferLayout)
		{
		}

		Mesh(const BufferLayout& layout)
			: mBufferLayout(layout) {}

		Mesh(const BufferLayout& layout,
			void* vertex_data, usize vertex_size,
			u32* index_data, usize index_count,
			MeshContext* context) :
					pContext(context),
					pVao(VertexArray::Create()),
					mBufferLayout(layout)
		{
			setVertexData(vertex_data, vertex_size);
			setIndexData(index_data, index_count);
		}

		Mesh& operator=(Mesh&& comp) noexcept
		{
			return *this;
		}

		void setVertexData(void* vertex_data, usize vertex_data_size);
		void setIndexData(u32* index_data, u32 index_count);

		MeshContext* pContext;
		Ref<VertexArray> pVao;
	private:
		const BufferLayout& mBufferLayout;
	};

	struct Model : Entity
	{
		Model(RegistryData* reg)
			: Entity(reg) {}

		std::vector<Mesh> Meshes;
	};

	struct PointLight : Model
	{
		PointLight(RegistryData* reg, LightContext&& context)
			: Model(reg)
		{
			AddComponent<LightContext>(std::move(context));
			AddComponent<TransformComponent>(glm::mat4(1.0f));
		}

		void changePosition(glm::vec3 new_position);
		void changeColor(glm::vec3 new_color);
	};

	// Returns vertices, normals, indices
	// Delete only vertices after usage.
	std::tuple<glm::vec3*, glm::vec3*, u32*> GenCubeSmoothVertices(glm::vec3 position, glm::vec3 dimensions);
}



#endif //__RENDERCOMPONENTS_HEADER__