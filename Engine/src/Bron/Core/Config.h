#pragma once

// Build configuration is supplied by CMake (see Engine/src/CMakeLists.txt), so
// that Debug and Release actually differ and so that non-default toolchains do
// not silently inherit a Windows/Debug-shaped build:
//
//   BR_COMPILE          - set while compiling Bron itself
//   BR_DEBUG            - Debug / RelWithDebInfo only
//   BR_RENDERER_OPENGL  - selected rendering backend
//
// Building without CMake is not supported.
#if !defined(BR_RENDERER_OPENGL)
	#error "No rendering API selected - configure the project through CMake."
#endif
