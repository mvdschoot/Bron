#ifndef BUILTIN_SHADERS_HEADER
#define BUILTIN_SHADERS_HEADER

#include <string>

namespace Steve
{
	// The shaders behind the engine's built-in renderers. They are not assets:
	// each one has a hard contract with the C++ that drives it (vertex layout,
	// uniform names, texture slots), so the engine owns them and keeps them in
	// its own translation unit. Nothing is read from disk, and there is no
	// engine asset folder to ship or copy next to the executable.
	namespace BuiltinShaders
	{
		enum class Id
		{
			Renderer2D,
			Grid,
			Phong3D,
			Text
		};

		// GLSL source for a built-in shader, ready for Shader::Create.
		std::string Source(Id id);
	}
}

#endif // BUILTIN_SHADERS_HEADER
