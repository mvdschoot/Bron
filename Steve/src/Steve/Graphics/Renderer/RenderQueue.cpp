#include "RenderQueue.h"

namespace Steve
{
	RenderQueue::RenderQueue()
	{
	}


	void RenderQueue::addModel(Ref<Model> model)
	{
		for(Ref<Mesh>& m : model->GetMeshes())
		{
			Ref<MaterialBase> material = m->getMaterial();
			pQueue[material->shaderName][material].push_back(m);
		}
	}

}

