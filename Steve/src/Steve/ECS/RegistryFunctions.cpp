#include "RegistryFunctions.h"

#include "Entity.h"
#include "RegistryData.h"
#include "Handle.h"
#include "Group.h"

namespace Steve
{
	std::optional<const UUID> RegistryFunctions::IsInGroup(RegistryData * data, const std::type_index type)
	{
		auto it = data->GroupTypes.begin();
		for (; it != data->GroupTypes.end(); ++it)
		{
			auto it2 = it->second.begin();
			for (; it2 != it->second.end(); ++it2)
			{
				if (*it2 == type)
					return it->first;
			}
		}
		return {};
	}
	
	IHandle* RegistryFunctions::AddComponent(RegistryData* data, Entity* entity, IHandle* component)

	{
		CH_PROFILE_FUNCTION();
		// CORE_ASSERT(data->Entities.contains(entity->Id), "Entity does not exists so component cannot be added")
		CORE_ASSERT(!entity->Contains(component->Type), "Entity already has a component of this type")

		// Inserts handle and gets pointer to it
		IHandle* new_handle = &data->ComponentHandles.emplace(component->Id, std::move(*component));

		entity->ComponentIds.emplace(component->Type, &component->Id);
		entity->ComponentHandles.emplace(component->Id, new_handle);

		const std::optional<const UUID> group_id = IsInGroup(data, new_handle->Type);

		// If type not in a group or the entity does not have all types for the group
		if (!group_id.has_value() || !entity->ContainsAll(data->GroupTypes.at(*group_id))) {
			data->RestComponents.Insert(new_handle);
		}
		else {
			data->Groups.at(*group_id)->InsertNew(*entity);
		}

		return new_handle;
	}

	void RegistryFunctions::DestroyComponent(RegistryData* data, Entity* entity, std::type_index type)
	{
		IHandle* component_handle = entity->GetComponent(type);

		// Not in group
		if (data->RestComponents.Contains(component_handle->Id))
		{
			data->RestComponents.Remove(component_handle->Id);
		}
		else  // Group
		{
			auto opt = IsInGroup(data, component_handle->Type);
			CORE_ASSERT(opt.has_value(), "Component is nowhere")

			data->RestComponents.Insert();
		}

		data->ComponentHandles.remove(component_handle->Id);
	}

	Entity* RegistryFunctions::CreateEntity(RegistryData* data)
	{
		auto* ent = new Entity(data);
		data->Entities.emplace(ent->Id, std::move(ent));
		return data->Entities[ent->Id];
	}

	void RegistryFunctions::AddEntity(RegistryData* data, Entity* entity)
	{
		data->Entities.emplace(entity->Id, std::move(entity));
	}

	void RegistryFunctions::ResizeIfNeeded(RegistryData* data)
	{

	}
}
