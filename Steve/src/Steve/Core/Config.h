#ifndef __CONFIG_HEADER____
#define __CONFIG_HEADER____

// Build configuration is supplied by CMake (see Steve/src/CMakeLists.txt), so
// that Debug and Release actually differ and so that non-default toolchains do
// not silently inherit a Windows/Debug-shaped build:
//
//   CH_COMPILE     - set while compiling Steve itself
//   CH_DEBUG       - Debug / RelWithDebInfo only
//   CH_API_OPENGL  - selected rendering backend
//
// Building without CMake is not supported.
#if !defined(CH_API_OPENGL)
	#error "No rendering API selected - configure the project through CMake."
#endif

#endif
