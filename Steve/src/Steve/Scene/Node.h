#ifndef __NODE_HEADER__
#define __NODE_HEADER__

#include "Steve/Core/Core.h"

#include "Steve/ECS/Entity.h"
#include "Steve/Graphics/Components/MiscellaneousComponents.h"

namespace Steve
{
	enum NodeType_
	{
		NodeType_Mesh = BIT(0),
		NodeType_Model = BIT(1),
		NodeType_PointLight = BIT(2),

		NodeType_Object = BIT(3)
	};

	class Node : public Entity
	{
	public:
		Node(RegistryData* reg) : Entity(reg)
		{
			AddComponent<TransformComponent>();
		}
 
		void AddChildNode(Node* node, bool add_to_reg = false);
		void RemoveChildNode(Node* node);

		// depth 1 means only local transform.
		glm::mat4 GetTransform(i16 depth = 0);

		std::string name = "Default name";
		u64 type = 0;
		std::vector<Node*> children;
		Node* parent = nullptr;
	};
}

#endif