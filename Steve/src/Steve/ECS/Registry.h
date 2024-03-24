#ifndef __RRREGISTRY_HEADER__
#define __RRREGISTRY_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/UUID.h"

#include "Group.h"
#include "Handle.h"
#include "RegistryData.h"
#include "RegistryFunctions.h"

#include <optional>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <utility>


namespace Steve
{


	class Registry
	{
	public:
		Registry() : pData{ {5000} } {}
        Registry(RegistryData&& reg_data) : pData(reg_data) {}
		~Registry() {}
        
        // Entity* CreateEntity();
        // void AddEntity(Entity* entity);
        // void DestroyEntity(Entity& entity);

		template<typename ...Ts>
        void GroupComponents()
        {
            CH_PROFILE_FUNCTION();

            // Check if any component already in a group
            bool isInGroup = false;

            bool result[] = {false, (RegistryFunctions::IsInGroup(&pData, std::type_index(typeid(Ts))).has_value(), true)...};

            // (void(isInGroup = isInGroup || RegistryFunctions::IsInGroup(pData, std::type_index(typeid(Ts))).has_value()), ...);
            for (int x = 1; x < sizeof...(Ts); x++) {
                CORE_ASSERT(!result[x], "Component already in group")
            }

            Group<Ts...> group(&pData);
            group.SetAll();

            // At this point, group has copy's of the object
            // Next remove them from the old containers

            CH_PROFILE_FUNCTION();
            View<Ts...> view;
            for (std::tuple<Handle<Ts>&...>& tup : view) {
                std::apply(pData.RestComponents.Remove, tup.Id);
            }
        }


        RegistryData pData;

	};
}


#endif

