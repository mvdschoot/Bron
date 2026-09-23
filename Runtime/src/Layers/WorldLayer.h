#pragma once

#include "Bron.h"

#include "Bron/Game/Manifest.h"

#include <filesystem>

namespace bron::runtime {
/// The game itself: the one layer a shipped build has.
///
/// It owns the scene, ticks it, and draws it through whichever camera the scene marks
/// primary. There is no edit mode to leave - the runtime is always playing, which is the
/// one thing that makes it simpler than the editor rather than a subset of it.
class WorldLayer final : public Layer {
public:
	/// 'root' is where to look for the manifest. Empty means the executable's directory.
	explicit WorldLayer(std::filesystem::path root);

	void OnAttach() override;
	void OnUpdate(Timestep ts) override;
	void OnEvent(Event& event) override;

private:
	/// Reads the manifest beside the executable and loads the scene it names. Leaves
	/// scene_ null and logs when there is nothing to run.
	void Boot();

	/// As given on the command line, or empty for the executable's directory. Resolved in
	/// Boot() rather than here, because a constructor is no place to be touching the disk.
	std::filesystem::path root_;

	Scope<Scene> scene_;

	/// Whether the scene has been reported as undrawable. A scene with no camera cannot
	/// be drawn, and saying so sixty times a second would bury every other line in the
	/// log under it.
	bool warned_about_camera_ = false;
};
} // namespace bron::runtime
