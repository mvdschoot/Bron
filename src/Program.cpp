#include "Steve.h"

#include <memory>
#include <vector>

namespace LuigiMaker
{

	class SomeLayer : public ::Steve::Layer
	{
	public:
		SomeLayer(){};
		virtual ~SomeLayer(){};

		virtual void OnAttach()
		{
			CH_PROFILE_FUNCTION();
			Steveer::Init();
			_shaderlib.addShader(_shader_loc, _shader_name);

			_pos = {0.0f, 0.0f, -0.7};
			_rot = 0.0f;

			uint wwidth = Steve::Application::getWindow()->getWindowWidth();
			uint wheight = Steve::Application::getWindow()->getWindowHeight();

			_texture = Steve::Texture2D::Create(_texture_loc);
			_camera = Steve::createRef<Steve::OrthographicCamera>(-1.0f, 1.0f, -1.0f, 1.0f,
				(float)wwidth / (float)wheight, _pos, 0.0f);

			Steve::FramebufferSpecification spec;
			spec.width = wwidth;
			spec.height = wheight;
			_framebuffer = Steve::Framebuffer::Create(spec);
		}
		virtual void OnDetach()
		{
		}
		virtual void OnEvent(Steve::Event &event)
		{
			// APP_INFO("Event: {}", event.GetName());
			Steve::EventDispatcher e(event);
			e.Dispatch<Steve::MouseScrolledEvent>(BIND_EVENT_FN(SomeLayer::onMouseScrolled));
		}

		bool onMouseScrolled(Steve::MouseScrolledEvent& e)
		{
			_zoom -= e.getOffsetY() * 0.15f;
			_zoom = std::max(_zoom, 0.15f);

			_camera->setZoom(_zoom);
			return true;
		}

		void isKeyPressed(Steve::Timestep ts)
		{
			float dt = ts.getSeconds();
			if (Steve::Input::isKeyPressed(Steve::Key::A))
			{
				_pos.x -= _speed * dt;
			}
			if (Steve::Input::isKeyPressed(Steve::Key::D))
			{
				_pos.x += _speed * dt;
			}

			if (Steve::Input::isKeyPressed(Steve::Key::W))
			{
				_pos.y += _speed * dt;
			}

			if (Steve::Input::isKeyPressed(Steve::Key::S))
			{
				_pos.y -= _speed * dt;
			}
		}

		virtual void OnUpdate(Steve::Timestep ts)
		{
			_framebuffer->bind();

			CH_PROFILE_FUNCTION();
			_frame_times.push_back(ts.getMilliseconds());

			SteveerCommand::ClearColor({1.0f, 1.0f, 0.1f, 1.0f});
			SteveerCommand::clear();

			isKeyPressed(ts);
			_camera->setPosition(_pos);
			_camera->setRotation(_rot);
			Steveer2D::beginScene(*_camera);

			float size = 0.05;
			for(int x = 0; x < 100; x++){
				for(int y = 0; y < 1; y++) {
					Steveer2D::drawQuad({-1.0 + x * size, -1.0 + y * size},{size, size}, _texture);
				}
			}

			Steveer2D::endScene();

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

		Steve::ShaderLibrary _shaderlib;
		Steve::Ref<Steve::VertexArray> _vertex_array;
		Steve::Ref<Steve::Camera> _camera;
		glm::vec3 _pos;
		float _rot;
		std::vector<float> _frame_times;
		float _avg_frame_time;
		const float _speed = 1.0f;
		float _zoom = 1.0f;

		Steve::Ref<Steve::Framebuffer> _framebuffer;

		Steve::Ref<Steve::Texture> _texture;

		const std::string _texture_loc = "../../src/Assets/muscular_rick.png";
		std::string _shader_loc = "../../src/Assets/shader.glsl";
		std::string _shader_name = "main";
	};

	class Program : public Steve::Application
	{
	public:
		Program()
		{
			Steve::Overlay *overlay = new Steve::Overlay;
			overlay->insertLayer(new SomeLayer);
			addOverlay(overlay);
		}
		~Program(){};
	};

}

Steve::Application *Steve::createApplication()
{
	return new LuigiMaker::Program;
}
