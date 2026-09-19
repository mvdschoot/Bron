//
// Created by mathi on 19-9-2026.
//

#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Timestep.h"
#include "Bron/Core/UUID.h"
#include "Bron/Events/Event.h"

#include "entt/entity/entity.hpp"

#include <filesystem>


namespace bron {
class Scene;

namespace lua {
struct State;
struct Script;
struct ScriptInstance;

class LuaManager {
public:
	explicit LuaManager(Scene* scene);
	// Out of line, where State is a complete type.
	~LuaManager();

	LuaManager(const LuaManager&) = delete;
	LuaManager& operator=(const LuaManager&) = delete;

	UUID RegisterScript(const std::filesystem::path& location) const;
	void AttachScript(UUID script_id, entt::entity entity) const;

	void OnUpdate(Timestep ts) const;
	void OnStart();
	void OnDestroy();
	void OnEvent(Event& event);

private:
	Scene* scene_;
	Scope<State> state_;
};
} // namespace lua
} // namespace bron
