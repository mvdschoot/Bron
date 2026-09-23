#pragma once

#include "Core/Project.h"

#include <filesystem>

namespace bron::editor {
/// Writes a runnable game into 'destination': the runtime executable, the scene, every
/// asset it depends on and the manifest tying the three together.
///
/// Export copies, it does not compile. What comes out is the prebuilt runtime plus
/// project data plus a rule for finding that data, and the rule is the manifest - so
/// everything it names has to be written relative to the destination. An absolute path
/// in there is the classic export bug: it runs on the machine that produced it and
/// nowhere else.
///
/// The scene is written from memory rather than copied from disk, so the export matches
/// what is on screen and no unsaved work is silently left out. The project itself is
/// never modified.
///
/// Returns false and logs when the export is incomplete. A partial export is left where
/// it is: it is evidence about what failed, and deleting it would take any hand-placed
/// files in that folder with it.
bool ExportGame(const Project& project, const Scene& scene, const std::filesystem::path& destination);
} // namespace bron::editor
