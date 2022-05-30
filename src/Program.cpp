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
			CH_PROFILE_FUNCTION();
			_shaderlib.addShader(_shader_loc, _shader_name);

			_texture = Cheets::Texture2D::Create(_texture_loc);
			_camera = Cheets::createRef<Cheets::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
																	_pos, 0.0f);
		}
		virtual void OnDetach()
		{
		}
		virtual void OnEvent(Cheets::Event &event)
		{
			// APP_INFO("Event: {}", event.GetName());
			// Cheets::EventDispatcher e(event);
			// e.Dispatch<Cheets::KeyPressedEvent>(BIND_EVENT_FN(SomeLayer::onKeyPressed));
		}

		void isKeyPressed(Cheets::Timestep ts)
		{
			float dt = ts.getSeconds();
			if (Cheets::Input::isKeyPressed(Cheets::Key::A))
			{
				_pos.x -= _speed * dt;
			}
			if (Cheets::Input::isKeyPressed(Cheets::Key::D))
			{
				_pos.x += _speed * dt;
			}

			if (Cheets::Input::isKeyPressed(Cheets::Key::W))
			{
				_pos.y += _speed * dt;
			}

			if (Cheets::Input::isKeyPressed(Cheets::Key::S))
			{
				_pos.y -= _speed * dt;
			}
			if (Cheets::Input::isKeyPressed(Cheets::Key::Tab))
			{
				_pos.z -= _speed * dt;
			}
			if (Cheets::Input::isKeyPressed(Cheets::Key::CapsLock))
			{
				_pos.z += _speed * dt;
			}
		}

		virtual void OnUpdate(Cheets::Timestep ts)
		{
			CH_PROFILE_FUNCTION();
			Cheets::RendererCommand::ClearColor({1.0f, 0.1f, 0.1f, 1.0f});
			Cheets::RendererCommand::clear();

			isKeyPressed(ts);
			_camera->setPosition(_pos);
			_camera->setRotation(_rot);
			Cheets::Renderer2D::beginScene(*_camera);

			Cheets::Renderer2D::drawQuad({-0.5, -0.5},{1.0f, 1.0f}, _texture);

			Cheets::Renderer2D::endScene();
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