#include "RenderQueue.h"

namespace Steve
{
	RenderQueue::RenderQueue()
	{
	}


	void RenderQueue::addModel(Model* model)
	{
		for(Mesh* m : model->Meshes)
		{
			pQueue.pSet[m->pContext->pShader]
				.pSet[model]
				.pSet[m->pContext->pMaterial]
				.push_back(m);
		}
	}

}

