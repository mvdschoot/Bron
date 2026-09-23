#include "Layers/WorldLayer.h"

#include "Bron/Core/Platform.h"
#include "Bron/Scene/AssetManager.h"
#include "Bron/Scene/Serialization/Serialization.h"
#include "Bron/Scripting/LuaManager.h"
#include "Bron/Util/Paths.h"

namespace bron::runtime {
namespace {
// What the exporter writes beside the executable. The name is the contract between the
// two halves; nothing else looks for this file.
const char* k_manifest_name = "game.brongame";
} // namespace

void WorldLayer::OnAttach() {
	SceneRenderer::Init();
	Command::ClearColor({0.1f, 0.1f, 0.12f, 1.0f});

	Boot();
}

void WorldLayer::Boot() {
	const std::filesystem::path root = ExecutableDirectory();

	const std::optional<Manifest> manifest = Manifest::Load(root / k_manifest_name);
	if (!manifest) {
		// Load() has already said what was wrong with it. This is the line that says what
		// that means, because an empty window is not self-explanatory.
		BR_APP_ERROR("Nothing to run. Export a game beside this executable and start it again.");
		return;
	}

	// Asset paths in a scene file are stored relative to the asset root, so this has to
	// happen before anything is deserialized.
	const std::filesystem::path assets = (root / manifest->asset_directory).lexically_normal();
	paths::SetRoots(root, assets);
	bron::assets::AssetManager::Instance().Refresh();

	const std::filesystem::path scene_file = assets / manifest->startup_scene;
	if (!std::filesystem::exists(scene_file)) {
		BR_APP_ERROR("The startup scene {} is missing.", scene_file.string());
		return;
	}

	scene_ = CreateScope<Scene>();
	Serialization::DeserializeScene(*scene_, scene_file);

	scene_->OnRuntimeStart();

	BR_APP_INFO("Running {} from {}", manifest->name, scene_file.string());
}

void WorldLayer::OnUpdate(const Timestep ts) {
	// The runtime draws straight to the window rather than into a framebuffer a panel
	// owns, so the viewport is the window and has to follow it.
	const float width = static_cast<float>(Application::GetWindow()->GetWindowWidth());
	const float height = static_cast<float>(Application::GetWindow()->GetWindowHeight());
	Command::OnResize(0.0f, 0.0f, width, height);

	Command::Clear();
	Command::EnableDepth();

	if (!scene_)
		return;

	scene_->OnUpdate(ts);

	const entt::entity camera = scene_->PrimaryCamera();
	if (camera == entt::null) {
		if (!warned_about_camera_) {
			BR_APP_ERROR("This scene has no camera, so there is no way to look at it.");
			warned_about_camera_ = true;
		}
		return;
	}

	const CameraView view = ViewFrom(scene_->reg.get<CameraComponent>(camera), scene_->WorldTransform(camera),
									 height > 0.0f ? width / height : 1.0f);

	SceneRenderer::Draw(*scene_, view);
}

void WorldLayer::OnEvent(Event& event) { scene_->OnEvent(event); }

} // namespace bron::runtime
