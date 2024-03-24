#include "Entity.h"
#include "RegistryFunctions.h"
#include "Registry.h"

namespace Steve {



	IHandle* Entity::AddComponent(IHandle* component)
	{
		return RegistryFunctions::AddComponent(pRegData, this, component);
	}
}  // namespace Steve
