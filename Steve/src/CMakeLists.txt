cmake_minimum_required(VERSION 3.16)

add_library(Steve STATIC
	Steve/Core/Application.cpp 
	Steve/Core/Logger.cpp
	Steve/Core/Window.cpp

	Steve/Graphics/Renderer/API.cpp
	"Steve/Graphics/Renderer/Command.cpp"
	Steve/Graphics/Renderer/2D.cpp
	Steve/Graphics/Renderer/RenderQueue.cpp
	Steve/Graphics/Renderer/SceneRenderer.cpp
	
	Steve/Graphics/Renderer/Grid.cpp
	
	
	Steve/Graphics/Components/Mesh.cpp
	Steve/Graphics/Components/MiscellaneousComponents.cpp
	Steve/Graphics/Components/Model.cpp
	
	Steve/Scene/Scene.cpp
	Steve/Scene/Node.cpp

	Steve/Graphics/GraphicsContext.cpp
	Steve/Graphics/Shader.cpp
	
	Steve/Graphics/OrthographicCamera.cpp
	Steve/Graphics/VertexArray.cpp
	Steve/Graphics/Buffer.cpp
	Steve/Graphics/Texture.cpp
	Steve/Graphics/Framebuffer.cpp
	# Steve/Graphics/TextRenderer.cpp
	Steve/Graphics/FrustumCamera.cpp
	
	Steve/ECS/Entity.cpp
	Steve/ECS/RegistryFunctions.cpp
	

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
	 
	Platform/OpenGL/OpenGLContext.cpp
	Platform/OpenGL/OpenGLBuffer.cpp
	Platform/OpenGL/OpenGLVertexArray.cpp
	Platform/OpenGL/OpenGLRendererAPI.cpp
	Platform/OpenGL/OpenGLShader.cpp
	Platform/OpenGL/OpenGLTexture.cpp
	Platform/OpenGL/OpenGLFramebuffer.cpp
	Platform/OpenGL/OpenGLTextTexture.cpp

	Platform/Linux/LinuxWindow.cpp
	Platform/Linux/LinuxInput.cpp
		Steve/Graphics/ShaderRegistry.cpp
		Steve/Graphics/Phong/PhongMaterial.cpp
		Steve/Graphics/Components/Cube.cpp
		Steve/Graphics/Components/ModelLoader.cpp
		Steve/Graphics/Phong/PhongCube.cpp
        Steve/Graphics/LightManagement.cpp
)

set_property(TARGET Steve PROPERTY CXX_STANDARD 20)
target_include_directories(Steve PUBLIC .)