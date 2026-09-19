//
// Created by mathi on 19-9-2026.
//

#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Timestep.h"
#include "Bron/Events/Event.h"

#include "entt/entity/entity.hpp"

#include <string>


namespace bron {
class Scene;

class LuaManager {
public:
	explicit LuaManager(Scene* scene);
	// Out of line, where State is a complete type.
	~LuaManager();

	LuaManager(const LuaManager&) = delete;
	LuaManager& operator=(const LuaManager&) = delete;

	void RegisterScript(std::string location, entt::entity entity);
	void OnUpdate(Timestep ts);
	void OnAttach();
	void OnDetach();
	void OnEvent(Event& event);

private:
	// Holds the sol::state and everything else of sol's. Defined in LuaManager.cpp,
	// so including this header does not pull sol.hpp into the rest of the engine.
	struct State;

	Scene* scene_;
	Scope<State> state_;
};

} // namespace bron
