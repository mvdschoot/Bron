#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Scene/Components.h"
#include "Bron/Util/Util.h"

#include <entt/entity/registry.hpp>

#include <type_traits>
#include <utility>

// Enforces the constraints components declare through ComponentTraits:
//  - Requires:            components that must be on the same entity; added along automatically.
//  - Conflicts:           components that may not be on the same entity, in either direction.
//  - ParentRequiresAnyOf: the entity's parent must carry at least one of these.
//
// Everything works on a plain registry; Scene wraps it as AddComponent / RemoveComponent.

namespace bron {
/// A type the constraint checks know about: listed in AllComponents, with a complete set of
/// traits. A component missing from AllComponents would be silently skipped by every check
/// that walks the list, so these functions refuse it outright.
template<typename T>
concept Component =
		InList<T, AllComponents> && SpecializationOf<typename ComponentTraits<T>::Requires, ComponentList> &&
		SpecializationOf<typename ComponentTraits<T>::Conflicts, ComponentList> &&
		SpecializationOf<typename ComponentTraits<T>::ParentRequiresAnyOf, ComponentList>;

namespace restriction {
namespace detail {
template<typename... Cs>
constexpr bool AllAreComponents(ComponentList<Cs...>) {
	return (Component<Cs> && ...);
}

// Checks every trait specialization up front, used or not - a specialization that forgets to
// derive from DefaultComponentTraits fails here instead of at its first use.
static_assert(AllAreComponents(AllComponents{}), "A component in AllComponents has incomplete ComponentTraits");

inline entt::entity ParentOf(const entt::registry& reg, const entt::entity e) {
	const HierarchyComponent* hierarchy = reg.try_get<HierarchyComponent>(e);
	return hierarchy != nullptr ? hierarchy->parent : entt::null;
}

template<Component... Cs>
bool HasAny(const entt::registry& reg, const entt::entity e, ComponentList<Cs...>) {
	return reg.any_of<Cs...>(e);
}

// Whether a component already on 'e' lists T as a conflict - the reverse of T's own list.
template<Component T, Component... Cs>
bool IsConflictedBy(const entt::registry& reg, const entt::entity e, ComponentList<Cs...>) {
	return ((InList<T, typename ComponentTraits<Cs>::Conflicts> && reg.all_of<Cs>(e)) || ...);
}

template<Component T, Component... Cs>
bool IsRequired(const entt::registry& reg, const entt::entity e, ComponentList<Cs...>) {
	return ((InList<T, typename ComponentTraits<Cs>::Requires> && reg.all_of<Cs>(e)) || ...);
}

template<Component... Ps>
bool ParentSatisfies(const entt::registry& reg, const entt::entity parent, ComponentList<Ps...>) {
	// An empty list is no rule at all, so any parent - or none - will do.
	return sizeof...(Ps) == 0 || (parent != entt::null && reg.any_of<Ps...>(parent));
}

/// ParentSatisfies, as if T were already removed from 'parent'.
/// To check if the parent will be upheld if T is removed.
template<Component T, Component... Ps>
bool ParentSatisfiesWithout(const entt::registry& reg, const entt::entity parent, ComponentList<Ps...>) {
	return ((!std::is_same_v<T, Ps> && reg.all_of<Ps>(parent)) || ...);
}

template<Component T, Component... Cs>
bool ChildAcceptsParentWithout(const entt::registry& reg, const entt::entity child, const entt::entity parent,
							   ComponentList<Cs...>) {
	// Only a child component whose parent rule mentions T can be broken by removing T.
	return ((!InList<T, typename ComponentTraits<Cs>::ParentRequiresAnyOf> || !reg.all_of<Cs>(child) ||
			 ParentSatisfiesWithout<T>(reg, parent, typename ComponentTraits<Cs>::ParentRequiresAnyOf{})) &&
			...);
}
} // namespace detail

/// Adding T also adds whatever T requires. It is refused when T is already there, when it
/// conflicts with a component on the entity (in either direction), when the entity's parent
/// does not satisfy T's parent rule, or when any of that holds for a requirement.
template<Component T>
[[nodiscard]] bool CanAdd(const entt::registry& reg, entt::entity e);

/// Removing T is refused when another component on the entity requires it, or when a child
/// relies on T for its parent rule and the entity has no other component that satisfies it.
template<Component T>
[[nodiscard]] bool CanRemove(const entt::registry& reg, const entt::entity e) {
	if (!reg.all_of<T>(e) || detail::IsRequired<T>(reg, e, AllComponents{}))
		return false;

	if (const HierarchyComponent* hierarchy = reg.try_get<HierarchyComponent>(e)) {
		for (const entt::entity child: hierarchy->children) {
			if (!detail::ChildAcceptsParentWithout<T>(reg, child, e, AllComponents{}))
				return false;
		}
	}
	return true;
}

/// Adds T and, first, whatever it requires. Asserts CanAdd - check it beforehand where the
/// answer can be 'no', since the assert is gone in release builds.
template<Component T, typename... Args>
T& Add(entt::registry& reg, entt::entity e, Args&&... args);

/// Removes T. Asserts CanRemove, like Add.
template<Component T>
void Remove(entt::registry& reg, const entt::entity e) {
	BR_CORE_ASSERT(CanRemove<T>(reg, e), "Removing the component from entity {} breaks a component constraint",
				   static_cast<u64>(e));
	reg.remove<T>(e);
}

// CanAdd and Add recurse into the requirements, so the helpers that do so are declared after
// the two they call.
namespace detail {
template<Component... Rs>
bool CanAddMissing(const entt::registry& reg, const entt::entity e, ComponentList<Rs...>) {
	return ((reg.all_of<Rs>(e) || CanAdd<Rs>(reg, e)) && ...);
}

template<Component... Rs>
void AddMissing(entt::registry& reg, const entt::entity e, ComponentList<Rs...>) {
	// Through Add, so each requirement brings its own requirements along.
	((reg.all_of<Rs>(e) ? void() : void(Add<Rs>(reg, e))), ...);
}
} // namespace detail

template<Component T>
bool CanAdd(const entt::registry& reg, const entt::entity e) {
	using Traits = ComponentTraits<T>;

	return !reg.all_of<T>(e) && !detail::HasAny(reg, e, typename Traits::Conflicts{}) &&
		   !detail::IsConflictedBy<T>(reg, e, AllComponents{}) &&
		   detail::ParentSatisfies(reg, detail::ParentOf(reg, e), typename Traits::ParentRequiresAnyOf{}) &&
		   detail::CanAddMissing(reg, e, typename Traits::Requires{});
}

template<Component T, typename... Args>
T& Add(entt::registry& reg, const entt::entity e, Args&&... args) {
	BR_CORE_ASSERT(CanAdd<T>(reg, e), "Adding the component to entity {} breaks a component constraint",
				   static_cast<u64>(e));
	detail::AddMissing(reg, e, typename ComponentTraits<T>::Requires{});
	return reg.emplace<T>(e, std::forward<Args>(args)...);
}
} // namespace restriction
} // namespace bron
