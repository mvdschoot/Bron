//
// Created by mathi on 19-9-2026.
//

#include "LuaManager.h"

#include "LuaRegister.h"

#include <sol/sol.hpp>

namespace bron {

struct LuaManager::State {
	sol::state lua;
};

LuaManager::LuaManager(Scene* scene) : scene_(scene), state_(CreateScope<State>()) {
	// Libraries first: RegisterMath adds to the math table open_libraries creates.
	state_->lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string, sol::lib::table);
	lua::RegisterAll(state_->lua, *scene_);
}

LuaManager::~LuaManager() = default;

} // namespace bron
