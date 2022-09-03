#include "Registry.h"

namespace Cheets
{
	Registry::Registry()
	{
	}

	Registry::~Registry()
	{
	}

	UUID& Registry::CreateEntity()
	{
		Entity ent;
		m_Entities.emplace(&(ent.p_Id), ent);
		return ent.p_Id;
	}


}
