#include "Cheets.h"

#include <memory>

namespace LuigiMaker
{

	class SomeLayer : public ::Cheets::Layer
	{
	public:
		SomeLayer(){};
		virtual ~SomeLayer(){};

		virtual void OnAttach()
		{
			_shader = Cheets::Shader::CreateShaderFromLocation(_v_shader_loc, _f_shader_loc);

			float vertices[4 * 7] = {
				-0.5f,-0.5f,0.0f, 	1.0f, 0.0f, 0.0f, 1.0f,
				0.5f,-0.5f,0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
				-0.5f,0.5f,0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
				0.5f,0.5f,0.0f,		0.0f, 0.0f, 0.0f, 1.0f
			};
			std_int indices[6] = {
				0, 1, 2,
				1, 2, 3
			};


			Cheets::Ref<Cheets::VertexBuffer> _vertex_buffer = Cheets::VertexBuffer::Create(vertices, sizeof(vertices));
			Cheets::Ref<Cheets::IndexBuffer> _index_buffer = Cheets::IndexBuffer::Create(indices, 6);

			std::string posName = "a_Position";
			std::string colorName = "a_Color";
			Cheets::BufferLayout layout =
			{
				Cheets::BufferElement{posName, Cheets::ShaderDataType::Float3},
				Cheets::BufferElement{colorName, Cheets::ShaderDataType::Float4}
			};

			_vertex_buffer->setBufferLayout(layout);

			_vertex_array = Cheets::VertexArray::Create();
			_vertex_array->addVertexBuffer(_vertex_buffer);
			_vertex_array->setIndexBuffer(_index_buffer);
		
			_camera = Cheets::OrthographicCamera(-1.0f, 1.0f, -1.0f, 1.0f,
					glm::vec3(0.0f, 0.0f, 1.0f), 0.0f);
		}
		virtual void OnDetach()
		{
		}
		virtual void OnEvent(Cheets::Event &event)
		{
		}
		virtual void OnUpdate()
		{
			Cheets::RendererCommand::ClearColor({1.0f, 0.1f, 0.1f, 1.0f});
			Cheets::RendererCommand::clear();

			Cheets::Renderer::beginScene(_camera);
			
			Cheets::Renderer::submit(_shader, _vertex_array);

			Cheets::Renderer::endScene();
		}
		virtual void OnImGuiRender()
		{
		}

	private:
		Cheets::Ref<Cheets::Shader>_shader;
		Cheets::Ref<Cheets::VertexArray> _vertex_array;
		Cheets::Camera _camera;

		const std::string _v_shader_loc = "../src/Assets/shader.vert";
		const std::string _f_shader_loc = "../src/Assets/shader.frag";
	};

	class Program : public Cheets::Application
	{
	public:
		Program()
		{
			Cheets::Overlay *overlay = new Cheets::Overlay;
			overlay->insertLayer(new SomeLayer);
			addOverlay(overlay);
		}
		~Program(){};
	};

}

Cheets::Application *Cheets::createApplication()
{
	return new LuigiMaker::Program;
}