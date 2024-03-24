#ifndef ENTITY_HEADER_
#define ENTITY_HEADER_

#include "Steve/Core/Core.h"
#include "Steve/Core/Logger.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/UUID.h"

#include "Handle.h"
#include "RegistryData.h"

#include <map>
#include <unordered_map>
#include <tuple>
#include <typeindex>
#include <type_traits>
#include <utility>


namespace Steve
{
	// Only contains 1 of a component type
	class Entity
	{
	public:

		Entity(RegistryData* regData) : pRegData(regData)
		{}
		~Entity() = default;

		// Entity(const Entity& ent) = default;

		bool operator==(const Entity& other){return Id == other.Id;}


		template<typename T>
		[[nodiscard]] Handle<T>& GetComponent()
		{
			const std::type_index type(typeid(T));

			const UUID& id = ComponentIds.at(type);
			auto res = ComponentHandles.at(id);
			return *(Handle<T>*)res;
		}

		[[nodiscard]] IHandle* GetComponent(const std::type_index type)
		{
			return ComponentHandles.at(ComponentIds[type]);
		}

		template<typename ...Ts>
		[[nodiscard]] std::tuple<Handle<Ts>&...> GetComponents()
		{
			std::tuple<Handle<Ts>&...> res;
			ApplyToTuple(res, [&, this]<typename T>(T& handle)
				{
					handle = GetComponent<typename RemoveHandle<T>::type>();
				});
			return res;
		}

		// Checks if any of the types are here
		template<typename ...T>
		[[nodiscard]] bool ContainsAll() const
		{
			CH_PROFILE_FUNCTION();

			return ([&, this]<typename... Args>() {
				return (Contains(std::type_index(typeid(Args))) && ...);
			})();
		}
		
		[[nodiscard]] bool ContainsAll(std::vector<std::type_index>& types) const
		{
			CH_PROFILE_FUNCTION();

			for (std::type_index& type : types)
			{
				if (ComponentIds.contains(type))
					return false;
			}

			return true;
		}

		template<typename T>
		[[nodiscard]] bool Contains() const
		{
			return ComponentIds.contains(std::type_index(typeid(T)));
		}
		
		[[nodiscard]] bool Contains(std::type_index type) const
		{
			return ComponentIds.contains(type);
		}

		// Shortcut to registry function
		template<typename T>
		Handle<T>& AddComponent(T&& component)
		{
			Handle<T> handle(component);
			return *(Handle<T>*)AddComponent((IHandle*) & handle);
		}

		template<typename T, typename ...Ts>
		Handle<T>& AddComponent(Ts&&... args)
		{
			return AddComponent(T(args...));
		}
		

		// Shortcut to registry function
		// template<typename T>
		// void DestroyComponent() { RegistryFunctions::DestroyComponent(std::type_index(typeid(T))); }


	private:
		IHandle* AddComponent(IHandle* component);
		void RemovedComponent(IHandle* handle)
		{

			ComponentIds.erase(handle->Type);
			ComponentHandles.erase(handle->Id);
		}

	public:
		const UUID Id;
		std::unordered_map<std::type_index, const UUID> ComponentIds;
		std::unordered_map<const UUID, IHandle*> ComponentHandles;

	protected:
		RegistryData* pRegData;
	};
}


#endif

