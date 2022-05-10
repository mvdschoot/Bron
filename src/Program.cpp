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
			_shaderlib.addShader(_shader_loc, _shader_name);

			float vertices[4 * 7] = {
				-0.5f,-0.5f,0.0f, 	0.0f, 0.0f,
				0.5f,-0.5f,0.0f,	1.0f, 0.0f,
				-0.5f,0.5f,0.0f,	0.0f, 1.0f,
				0.5f,0.5f,0.0f,		1.0f, 1.0f
			};
			std_int indices[6] = {
				0, 1, 2,
				1, 2, 3
			};

			_pos = glm::vec3(0.0f, 0.0f, 1.0f);

			Cheets::Renderer::Init();

			_texture = Cheets::Texture2D::Create(_texture_loc);
			_texture->bind(0);
			_shaderlib.getShader(_shader_name)->setUniform1i("text", 0);

			Cheets::Ref<Cheets::VertexBuffer> _vertex_buffer = Cheets::VertexBuffer::Create(vertices, sizeof(vertices));
			Cheets::Ref<Cheets::IndexBuffer> _index_buffer = Cheets::IndexBuffer::Create(indices, 6);

			Cheets::BufferLayout layout =
			{
				Cheets::BufferElement{"a_Position", Cheets::ShaderDataType::Float3},
				Cheets::BufferElement{"a_uv", Cheets::ShaderDataType::Float2}
			};

			_vertex_buffer->setBufferLayout(layout);

			_vertex_array = Cheets::VertexArray::Create();
			_vertex_array->addVertexBuffer(_vertex_buffer);
			_vertex_array->setIndexBuffer(_index_buffer);
		
			_camera = Cheets::createRef<Cheets::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
					_pos, 0.0f);
		}
		virtual void OnDetach()
		{
		}
		virtual void OnEvent(Cheets::Event &event)
		{
			//APP_INFO("Event: {}", event.GetName());
			//Cheets::EventDispatcher e(event);
			//e.Dispatch<Cheets::KeyPressedEvent>(BIND_EVENT_FN(SomeLayer::onKeyPressed));
		}

		void isKeyPressed(Cheets::Timestep ts)
		{
			float dt = ts.getSeconds();
			if(Cheets::Input::isKeyPressed(Cheets::Key::A))
			{
				_pos.x -= _speed * dt;
			}
			if(Cheets::Input::isKeyPressed(Cheets::Key::D))
			{	
				_pos.x += _speed * dt;
			}

			if(Cheets::Input::isKeyPressed(Cheets::Key::W))
			{	
				_pos.y += _speed * dt;
			}

			if(Cheets::Input::isKeyPressed(Cheets::Key::S))
			{	
				_pos.y -= _speed * dt;
			}
			if(Cheets::Input::isKeyPressed(Cheets::Key::Tab))
			{
				_pos.z -= _speed * dt;
			}
			if(Cheets::Input::isKeyPressed(Cheets::Key::CapsLock))
			{
				_pos.z += _speed * dt;
			}
		}

		virtual void OnUpdate(Cheets::Timestep ts)
		{
			Cheets::RendererCommand::ClearColor({1.0f, 0.1f, 0.1f, 1.0f});
			Cheets::RendererCommand::clear();

			isKeyPressed(ts);
			_camera->setPosition(_pos);
			_camera->setRotation(_rot);
			Cheets::Renderer::beginScene(*_camera);
			
			Cheets::Renderer::submit(_shaderlib.getShader(_shader_name), _vertex_array);

			Cheets::Renderer::endScene();
		}

		virtual void OnImGuiRender()
		{
		}

	private:
		Cheets::ShaderLibrary _shaderlib;
		Cheets::Ref<Cheets::VertexArray> _vertex_array;
		Cheets::Ref<Cheets::Camera> _camera;
		glm::vec3 _pos;
		float _rot;
		const float _speed = 1.0;

		Cheets::Ref<Cheets::Texture> _texture;

		const std::string _texture_loc = "../src/Assets/muscular_rick.png";
		std::string _shader_loc = "../src/Assets/shader.glsl";
		std::string _shader_name = "main";
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