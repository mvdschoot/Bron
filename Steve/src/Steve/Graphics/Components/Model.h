#ifndef __MODEL_JEADR__
#define __MODEL_JEADR__


#include "MiscellaneousComponents.h"
#include "Mesh.h"

#include "Steve/Scene/Node.h"

namespace Steve
{
	struct Model : public Node
	{
		Model(RegistryData* reg)
			: Node(reg)
		{
			type |= NodeType_Model;
		}

		std::vector<Mesh*> Meshes;


		virtual glm::vec3 GetPosition();

		glm::vec3 GetCentroid() const;

		template <typename ... Ts>
		Mesh* AddMesh(Ts&&... args)
		{
			Mesh* m = new Mesh(args...);
			AddChildNode(m, true);
			Meshes.push_back(m);
			return m;
		}
	};


}

#endif