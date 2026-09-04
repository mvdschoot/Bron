cmake_minimum_required(VERSION 3.16)

add_library(Bron STATIC
	# Core
	Bron/Core/Application.cpp
	Bron/Core/Logger.cpp
	Bron/Core/Window.cpp

	# Layers
	Bron/Layers/Layer.cpp
	Bron/Layers/Overlay.cpp
	Bron/Layers/OverlayStack.cpp
	Bron/Layers/ImGuiLayer.cpp

	# Graphics
	Bron/Graphics/GraphicsContext.cpp
	Bron/Graphics/Shader.cpp
	Bron/Graphics/ShaderRegistry.cpp
	Bron/Graphics/BuiltinShaders.cpp
	Bron/Graphics/VertexArray.cpp
	Bron/Graphics/Buffer.cpp
	Bron/Graphics/Texture.cpp
	Bron/Graphics/Framebuffer.cpp
	Bron/Graphics/LightManagement.cpp
	Bron/Graphics/OrthographicCamera.cpp
	Bron/Graphics/FrustumCamera.cpp
	Bron/Graphics/Phong/PhongMaterial.cpp
	Bron/Graphics/Components/Cube.cpp
	Bron/Graphics/Components/ModelLoader.cpp
	# TextRenderer is out of the build: it calls an R2D::DrawQuad overload that no
	# longer exists. TextTexture (Platform/OpenGL/OpenGLTextTexture.cpp) is still
	# built regardless, because Texture.cpp's TextTexture::Create names it.
	# Bron/Graphics/TextRenderer.cpp

	# Renderers
	Bron/Graphics/Renderer/API.cpp
	Bron/Graphics/Renderer/Command.cpp
	Bron/Graphics/Renderer/2D.cpp
	Bron/Graphics/Renderer/SceneRenderer.cpp
	Bron/Graphics/Renderer/Grid.cpp

	# Scene
	Bron/Scene/Scene.cpp
	Bron/Scene/Components.cpp
	Bron/Scene/Serialization/Serialization.cpp

	# Physics
	Bron/Physics/BVH.cpp
	Bron/Physics/Splitting.cpp
	Bron/Physics/Components.cpp
	Bron/Physics/System.cpp
	Bron/Physics/Collisions.cpp

	# Util
	Bron/Util/Util.cpp
	Bron/Util/Paths.cpp

	# OpenGL backend
	Platform/OpenGL/OpenGLContext.cpp
	Platform/OpenGL/OpenGLBuffer.cpp
	Platform/OpenGL/OpenGLVertexArray.cpp
	Platform/OpenGL/OpenGLRendererAPI.cpp
	Platform/OpenGL/OpenGLShader.cpp
	Platform/OpenGL/OpenGLTexture.cpp
	Platform/OpenGL/OpenGLFramebuffer.cpp
	Platform/OpenGL/OpenGLTextTexture.cpp

	# Desktop backend
	Platform/Desktop/DesktopWindow.cpp
	Platform/Desktop/DesktopInput.cpp)

target_include_directories(Bron PUBLIC .)

# cxx_std_20 as a compile feature (rather than the CXX_STANDARD property) makes
# the requirement mandatory and propagates it to anything linking Bron.
target_compile_features(Bron PUBLIC cxx_std_20)
set_target_properties(Bron PROPERTIES CXX_EXTENSIONS OFF)

# Build configuration that used to be hardcoded in Bron/Core/Config.h.
target_compile_definitions(Bron
	PUBLIC
		BR_RENDERER_OPENGL
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:BR_DEBUG>
	PRIVATE
		BR_COMPILE
)

if(MSVC)
	# /utf-8 makes MSVC read sources as UTF-8 like every other compiler;
	# /permissive- turns off the MSVC-only language extensions that let
	# non-portable code compile here but fail on GCC and Clang.
	target_compile_options(Bron PUBLIC /utf-8 /permissive-)
	target_compile_definitions(Bron PUBLIC
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		_CRT_SECURE_NO_WARNINGS
	)
else()
	target_compile_options(Bron PUBLIC -fvisibility=hidden)
endif()
