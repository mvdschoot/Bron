#include "Cheets.h"

#include <memory>
#include <vector>

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
			Cheets::Renderer::Init();
			_shaderlib.addShader(_shader_loc, _shader_name);

			_pos = {0.0f, 0.0f, -0.7};
			_rot = 0.0f;

			uint wwidth = Cheets::Application::getWindow()->getWindowWidth();
			uint wheight = Cheets::Application::getWindow()->getWindowHeight();

			_texture = Cheets::Texture2D::Create(_texture_loc);
			_camera = Cheets::createRef<Cheets::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
				(float)wwidth / (float)wheight, _pos, 0.0f);

			Cheets::FramebufferSpecification spec;
			spec.width = wwidth;
			spec.height = wheight;
			_framebuffer = Cheets::Framebuffer::Create(spec);
		}
		virtual void OnDetach()
		{
		}
		virtual void OnEvent(Cheets::Event &event)
		{
			// APP_INFO("Event: {}", event.GetName());
			Cheets::EventDispatcher e(event);
			e.Dispatch<Cheets::MouseScrolledEvent>(BIND_EVENT_FN(SomeLayer::onMouseScrolled));
		}

		bool onMouseScrolled(Cheets::MouseScrolledEvent& e)
		{
			_zoom -= e.getOffsetY() * 0.15f;
			_zoom = std::max(_zoom, 0.15f);

			_camera->setZoom(_zoom);
			return true;
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
		}

		virtual void OnUpdate(Cheets::Timestep ts)
		{
			_framebuffer->bind();

			CH_PROFILE_FUNCTION();
			_frame_times.push_back(ts.getMilliseconds());

			Cheets::RendererCommand::ClearColor({1.0f, 1.0f, 0.1f, 1.0f});
			Cheets::RendererCommand::clear();

			isKeyPressed(ts);
			_camera->setPosition(_pos);
			_camera->setRotation(_rot);
			Cheets::Renderer2D::beginScene(*_camera);

			float size = 0.05;
			for(int x = 0; x < 100; x++){
				for(int y = 0; y < 1; y++) {
					Cheets::Renderer2D::drawQuad({-1.0 + x * size, -1.0 + y * size},{size, size}, _texture);
				}
			}

			Cheets::Renderer2D::endScene();

			_framebuffer->unbind();
		}

		virtual void OnImGuiRender()
		{
			ImGui::Begin("Profiling Layer");
			if (_frame_times.size() == _fps_num_frames){
				_avg_frame_time = 0.0f;
				for(std::vector<float>::iterator x = _frame_times.begin(); x != _frame_times.end(); ++x){
					_avg_frame_time += (*x / _fps_num_frames);
				}
				_frame_times.clear();
			}
			ImGui::Text("Frame Time: %f (FPS %d)", _avg_frame_time, (int)(1.0f / _avg_frame_time * 1000.0f));

			uint32_t textureID = _framebuffer->getColorAttachID();
			ImGui::Image((void*)(intptr_t)textureID, ImVec2(355.0f, 200.0f), {0, 1}, {1, 0});
			ImGui::End();
		}

	private:
		const int _fps_num_frames = 10;

		Cheets::ShaderLibrary _shaderlib;
		Cheets::Ref<Cheets::VertexArray> _vertex_array;
		Cheets::Ref<Cheets::Camera> _camera;
		glm::vec3 _pos;
		float _rot;
		std::vector<float> _frame_times;
		float _avg_frame_time;
		const float _speed = 1.0f;
		float _zoom = 1.0f;

		Cheets::Ref<Cheets::Framebuffer> _framebuffer;

		Cheets::Ref<Cheets::Texture> _texture;

		const std::string _texture_loc = "../../src/Assets/muscular_rick.png";
		std::string _shader_loc = "../../src/Assets/shader.glsl";
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
