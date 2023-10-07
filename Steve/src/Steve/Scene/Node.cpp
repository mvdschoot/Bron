#include "Node.h"

#include "Steve/ECS/RegistryFunctions.h"

namespace Steve
{
	void Node::AddChildNode(Node* node, bool add_to_reg)
	{
		if (add_to_reg)
		{
			RegistryFunctions::AddEntity(pRegData, node);
		}

		node->parent = this;

		children.push_back(node);
	}

	void Node::RemoveChildNode(Node* node)
	{
		for(u32 i = 0; i < children.size(); i++)
		{
			if (children[i]->Id == node->Id)
			{
				children.erase(children.begin() + i);
				return;
			}
		}
		CORE_ASSERT(false, "Entity not a child");
	}

	glm::mat4 Node::GetTransform(i16 depth)
	{
		TransformComponent& t = GetComponent<TransformComponent>();

		if (!parent || depth == 1)
			return *t;

		return parent->GetTransform(--depth) * *t;
	}
}
