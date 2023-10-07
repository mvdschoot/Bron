#include "Model.h"

namespace Steve
{
	glm::vec3 Model::GetPosition()
	{
		return GetComponent<TransformComponent>()->Position;
	}

	glm::vec3 Model::GetCentroid() const
	{
		glm::vec3 res(0.0f);

		for(Mesh* mesh : Meshes)
		{
			res += mesh->GetComponent<TransformComponent>()->Position;
		}

		return res / (float)Meshes.size();
	}
}
