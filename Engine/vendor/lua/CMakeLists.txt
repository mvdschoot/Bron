# Lua ships a Makefile rather than a CMakeLists, so the release archive is
# fetched and the target is defined here. lua.c and luac.c are the standalone
# interpreter and compiler, each with its own main(), so they stay out of the
# library.

include(FetchContent)

FetchContent_Declare(
        lua
        URL https://www.lua.org/ftp/lua-5.4.9.tar.gz
        URL_HASH SHA256=2335b6c582a52654f94612bf10d2f4672805d05329aa6568b1d8cd9e5c6fb8e6
)
FetchContent_MakeAvailable(lua)

set(LUA_SRC ${lua_SOURCE_DIR}/src)

add_library(lua STATIC
        # Core
        ${LUA_SRC}/lapi.c
        ${LUA_SRC}/lcode.c
        ${LUA_SRC}/lctype.c
        ${LUA_SRC}/ldebug.c
        ${LUA_SRC}/ldo.c
        ${LUA_SRC}/ldump.c
        ${LUA_SRC}/lfunc.c
        ${LUA_SRC}/lgc.c
        ${LUA_SRC}/llex.c
        ${LUA_SRC}/lmem.c
        ${LUA_SRC}/lobject.c
        ${LUA_SRC}/lopcodes.c
        ${LUA_SRC}/lparser.c
        ${LUA_SRC}/lstate.c
        ${LUA_SRC}/lstring.c
        ${LUA_SRC}/ltable.c
        ${LUA_SRC}/ltm.c
        ${LUA_SRC}/lundump.c
        ${LUA_SRC}/lvm.c
        ${LUA_SRC}/lzio.c

        # Auxiliary library
        ${LUA_SRC}/lauxlib.c

        # Standard libraries
        ${LUA_SRC}/lbaselib.c
        ${LUA_SRC}/lcorolib.c
        ${LUA_SRC}/ldblib.c
        ${LUA_SRC}/liolib.c
        ${LUA_SRC}/lmathlib.c
        ${LUA_SRC}/loadlib.c
        ${LUA_SRC}/loslib.c
        ${LUA_SRC}/lstrlib.c
        ${LUA_SRC}/ltablib.c
        ${LUA_SRC}/lutf8lib.c
        ${LUA_SRC}/linit.c
)

target_include_directories(lua PUBLIC
        ${LUA_SRC}
)

# luaconf.h detects Windows by itself; elsewhere the platform has to be named,
# as the upstream Makefile does, to get dlopen() for require and the POSIX
# variants of the os/io functions.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
    target_link_libraries(lua PUBLIC m ${CMAKE_DL_LIBS})
elseif (APPLE)
    target_compile_definitions(lua PRIVATE LUA_USE_MACOSX)
endif ()
