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
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"
#include "Steve/Graphics/Renderer/2D.h"


namespace Steve
{
	enum class ShadingTech
	{
		PHONG,
		PBR
	};

	enum class TextureTypes
	{
		Diffuse,
		Specular,
		Normal
	};

	enum class MaterialDataTypes
	{
		AmbientFactor,
		Diffuse,
		Specular,
		Shininess,
		ShininessStrength,

		// The following are floats/integers to the texture slot
		DiffuseTexture,
		SpecularTexture,
		NormalTexture
	};

	enum class LightDataTypes
	{
		Position,
		Color
	};

	inline static MaterialDataTypes TextureConverter(TextureTypes type)
	{
		if (type == TextureTypes::Diffuse) return MaterialDataTypes::DiffuseTexture;
		if (type == TextureTypes::Specular) return MaterialDataTypes::SpecularTexture;
		if (type == TextureTypes::Normal) return MaterialDataTypes::NormalTexture;
		CORE_ASSERT(false, "Texture not here mate")
	}
	inline static TextureTypes TextureConverter(MaterialDataTypes type)
	{
		if (type == MaterialDataTypes::DiffuseTexture) return TextureTypes::Diffuse;
		if (type == MaterialDataTypes::SpecularTexture) return TextureTypes::Specular;
		if (type == MaterialDataTypes::NormalTexture) return TextureTypes::Normal;
		CORE_ASSERT(false, "Texture not here mate")
	}

	struct TexturePack
	{
		std::map<TextureTypes, Ref<Texture>> Textures;
	};

	template<typename T>
	struct UniformLayout : public BufferLayout
	{
		UniformLayout(std::initializer_list<std::pair<T, BufferElement>> data)
			: Data({})
		{
			std::vector<BufferElement> b;
			for (std::pair<T, BufferElement> el : data) {
				Data.push_back(el.first);
				b.push_back(el.second);
			}

			SetElements(std::move(b));
		}

		void SetName(T data_type, std::string new_name)
		{
			for (int x = 0; x < Data.size(); x++)
			{
				if (Data[x] == data_type)
				{
					_buffer_elements[x].name = new_name;
				}
			}
		}

		[[nodiscard]] const BufferElement& GetElementData(T light_data_type) const
		{
			for (int x = 0; x < Data.size(); x++)
			{
				if (Data[x] == light_data_type)
				{
					return GetElements()[x];
				}
			}
			CORE_ASSERT(false, "Cannot find the uniform data type type you are looking for.")
		}

		std::vector<T> Data;
	};

	template<typename T>
	struct UniformData
	{
		UniformData(const UniformLayout<T>* layout) : Layout(layout), Data(new u8[layout->GetStride()])
		{}

		void Set(T type, const uint8_t* value)
		{
			const BufferElement& el = Layout->GetElementData(type);
			memcpy_s(Data + el.offset, el.size, value, el.size);
		}
		
		template<typename S> void Set(T type, S value)
		{
			const BufferElement& el = Layout->GetElementData(type);
			memcpy_s(Data + el.offset, el.size, &value, el.size);
		}

		u8* Get(T type) const
		{
			const BufferElement& el = Layout->GetElementData(type);
			return Data + el.offset;
		}

		template<typename S> S* Get(T type) const
		{
			const BufferElement& el = Layout->GetElementData(type);
			return (S*)(Data + el.offset);
		}

		const UniformLayout<T>* Layout;
		u8* Data;
	};

	struct Material : public UniformData<MaterialDataTypes>
	{
		TexturePack* Textures;
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


	class Mesh : public Entity
	{
	public:
		Mesh(RegistryData* reg, const BufferLayout* layout)
			: Entity(reg, MESH_ENTITY), mBufferLayout(layout)
		{
			AddComponent<TransformComponent>();
		}

		Mesh(RegistryData* reg, 
			const BufferLayout* layout,
			void* vertex_data, usize vertex_size,
			u32* index_data, usize index_count,
			MeshContext* context) :
					Entity(reg, MESH_ENTITY),
					pContext(context),
					pVao(VertexArray::Create()),
					mBufferLayout(layout)
		{
			setVertexData(vertex_data, vertex_size);
			setIndexData(index_data, index_count);
			AddComponent<TransformComponent>();
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
		const BufferLayout* mBufferLayout;
	};

	struct Model : public Entity
	{
		Model(RegistryData* reg)
			: Entity(reg, MODEL_ENTITY)
		{
			AddComponent<TransformComponent>();
		}

		std::vector<Mesh*> Meshes;

		
		virtual glm::vec3 GetPosition();

		template <typename ... Ts>
		Mesh* AddMesh(Ts&&... args)
		{
			Meshes.push_back(&CreateChildEntity<Mesh>(args...));
			return Meshes.back();
		}
	};


	struct Cube : public Model
	{
		virtual glm::vec3 GetColor() const;
		virtual void SetColor(glm::vec3 color);

	protected:
		Cube(RegistryData* reg)
			: Model(reg) {}
	};

#define LightData UniformData<LightDataTypes>
	struct PointLight : public Cube
	{
		// Override because the lightdata component also has to be changed.
		void SetColor(glm::vec3 color) override;
		void SetUniformPosition(glm::vec3 pos);

		PointLight(RegistryData* reg, LightData&& context);
	};

	// Returns vertices, normals, indices
	// Delete only vertices after usage.
	std::tuple<glm::vec3*, glm::vec3*, u32*> GenCubeSmoothVertices(glm::vec3 dimensions);

	// Returns vertices, normals, indices, #vertices/normals, #indices
	// Delete only vertices after usage.
	std::tuple<glm::vec3*, glm::vec3*, u32*, u32, u32> GenSphereSmoothVertices(glm::vec3 position, float radius, uint32_t accuracy);
}



#endif //__RENDERCOMPONENTS_HEADER__