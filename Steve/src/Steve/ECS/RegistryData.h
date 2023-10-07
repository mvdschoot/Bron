#ifndef SIZEDB_HEADER_
#define SIZEDB_HEADER_

#include <map>
#include <vector>

#include "Containers.h"

namespace Steve
{
	class IHandle;
	class IGroup;
	class UUID;
	class Entity;

	struct RegistryData
	{
		StaticMap<const UUID, IHandle> ComponentHandles;
		std::map<const UUID, Entity*> Entities;
		ComponentContainer RestComponents;

		std::map<const UUID, std::vector<std::type_index>> GroupTypes;
		std::map<const UUID, IGroup*> Groups;
	};
}

#endif // SIZEDB_HEADER_
