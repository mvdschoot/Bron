//
// Created by mathi on 19-9-2026.
//

#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Timestep.h"
#include "Bron/Core/UUID.h"
#include "Bron/Input/Event.h"
#include "Bron/Scene/Asset.h"

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

	/// Gives 'entity' the script at 'location', loading the file if this is the first
	/// entity to use it.
	///
	/// Loading and attaching are one call because there was never a reason to do one
	/// without the other: a loaded script with no instances does nothing, and there is no
	/// way to attach one that has not been loaded. Both halves are idempotent, so
	/// attaching the same script to the same entity twice is a no-op rather than a second
	/// instance.
	///
	/// A script whose file is missing or fails to execute is reported and skipped; the
	/// entity simply ends up without it.
	void AttachScript(const assets::AssetHandle& script_handle, entt::entity entity) const;

	void OnUpdate(Timestep ts) const;
	void OnStart();
	void OnDestroy();
	void OnEvent(Event& event);

	void SetReceiveMouseInput(bool receive_input) const;
	void SetReceiveKeyboardInput(bool receive_input) const;

private:
	Scene* scene_;
	Scope<State> state_;
};
} // namespace lua
} // namespace bron
