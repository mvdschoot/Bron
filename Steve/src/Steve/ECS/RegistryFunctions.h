#ifndef __REGISTRYFUNCTIONS_HEADER_
#define __REGISTRYFUNCTIONS_HEADER_

#include <optional>
#include <typeindex>

namespace Steve
{
	struct RegistryData;
	class UUID;
	class IHandle;
	template<typename T> class Handle;
	class Entity;

	class RegistryFunctions
	{
	public:
		RegistryFunctions() = delete;

		static std::optional<const UUID> IsInGroup(RegistryData* data, const std::type_index type);
		static IHandle* AddComponent(RegistryData* data, Entity* entity, IHandle* component);
		static void DestroyComponent(RegistryData* data, Entity* entity, std::type_index type);
		static Entity* CreateEntity(RegistryData* data);
		static void AddEntity(RegistryData* data, Entity* entity);
		static void ResizeIfNeeded(RegistryData* data);

	};
}




#endif