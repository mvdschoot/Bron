cmake_minimum_required(VERSION 3.16)

add_library(Steve STATIC
	Steve/Core/Application.cpp 
	Steve/Core/Logger.cpp
	Steve/Core/Window.cpp

	Steve/Graphics/Renderer/API.cpp
	"Steve/Graphics/Renderer/Command.cpp"
	Steve/Graphics/Renderer/2D.cpp
	Steve/Graphics/Renderer/SceneRenderer.cpp
	
	Steve/Graphics/Renderer/Grid.cpp
	
	
	
	Steve/Scene/Scene.cpp
	Steve/Scene/Components.cpp

	Steve/Graphics/GraphicsContext.cpp
	Steve/Graphics/Shader.cpp
	Steve/Graphics/BuiltinShaders.cpp
	
	Steve/Graphics/OrthographicCamera.cpp
	Steve/Graphics/VertexArray.cpp
	Steve/Graphics/Buffer.cpp
	Steve/Graphics/Texture.cpp
	Steve/Graphics/Framebuffer.cpp
	# Steve/Graphics/TextRenderer.cpp
	Steve/Graphics/FrustumCamera.cpp
	
	

	Steve/Layers/Layer.cpp
	Steve/Layers/Overlay.cpp
	Steve/Layers/OverlayStack.cpp
	Steve/Layers/ImGuiLayer.cpp
	
	Steve/Physics/BVH.cpp
	Steve/Physics/Splitting.cpp
	Steve/Physics/Components.cpp
	Steve/Physics/System.cpp
	Steve/Physics/Collisions.cpp

	Steve/Util/Util.cpp
	Steve/Util/Paths.cpp
	 
	Platform/OpenGL/OpenGLContext.cpp
	Platform/OpenGL/OpenGLBuffer.cpp
	Platform/OpenGL/OpenGLVertexArray.cpp
	Platform/OpenGL/OpenGLRendererAPI.cpp
	Platform/OpenGL/OpenGLShader.cpp
	Platform/OpenGL/OpenGLTexture.cpp
	Platform/OpenGL/OpenGLFramebuffer.cpp
	Platform/OpenGL/OpenGLTextTexture.cpp

	Platform/Desktop/DesktopWindow.cpp
	Platform/Desktop/DesktopInput.cpp
		Steve/Graphics/ShaderRegistry.cpp
		Steve/Graphics/Phong/PhongMaterial.cpp
		Steve/Graphics/Components/Cube.cpp
		Steve/Graphics/Components/ModelLoader.cpp
        Steve/Graphics/LightManagement.cpp
)

target_include_directories(Steve PUBLIC .)

# cxx_std_20 as a compile feature (rather than the CXX_STANDARD property) makes
# the requirement mandatory and propagates it to anything linking Steve.
target_compile_features(Steve PUBLIC cxx_std_20)
set_target_properties(Steve PROPERTIES CXX_EXTENSIONS OFF)

# Build configuration that used to be hardcoded in Steve/Core/Config.h.
target_compile_definitions(Steve
	PUBLIC
		CH_API_OPENGL
		$<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:CH_DEBUG>
	PRIVATE
		CH_COMPILE
)

# Application asset root, so nothing has to guess at the working directory.
# Overridable at runtime with STEVE_PROJECT_ASSETS. The engine itself needs no
# such root - its shaders live in Steve/Graphics/BuiltinShaders.cpp.
target_compile_definitions(Steve PUBLIC
	STEVE_PROJECT_ASSET_DIR="${STEVE_PROJECT_ASSET_DIR}"
)

if(MSVC)
	# /utf-8 makes MSVC read sources as UTF-8 like every other compiler;
	# /permissive- turns off the MSVC-only language extensions that let
	# non-portable code compile here but fail on GCC and Clang.
	target_compile_options(Steve PUBLIC /utf-8 /permissive-)
	target_compile_definitions(Steve PUBLIC
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		_CRT_SECURE_NO_WARNINGS
	)
else()
	target_compile_options(Steve PUBLIC -fvisibility=hidden)
endif()
