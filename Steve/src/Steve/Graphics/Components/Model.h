#ifndef __MODEL_JEADR__
#define __MODEL_JEADR__


#include "MiscellaneousComponents.h"
#include "Mesh.h"

namespace Steve
{
	struct Model : public Node
	{
		Model(RegistryData* reg, std::vector<Ref<Mesh>>&& meshes)
			: Node(reg), meshes(meshes)
		{
			type |= NodeType_Model;
		}

		virtual glm::vec3 GetPosition();
		glm::vec3 GetCentroid() const;
		std::vector<Ref<Mesh>>& GetMeshes() { return meshes; }

	private:
		std::vector<Ref<Mesh>> meshes;
	};


}

#endif