#include "RenderQueue.h"

namespace Steve::graphics
{
	RenderQueue::RenderQueue()
	{
	}

	/*
	 * Automatically groups it.
	 * Sorta assumes that 1 model is rendered with 1 shader.
	 * Makes sense, but badly programmed, need refactoring.
	 */
	void RenderQueue::addModel(Model* model)
	{
		for(Mesh& m : model->Meshes)
		{
			pQueue.pSet[m.pContext->pShader]
				.pSet[model]
				.pSet[m.pContext->pMaterial]
				.push_back(&m);
		}
	}

}

