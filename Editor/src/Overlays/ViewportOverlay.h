#pragma once

#include "Core/EditorContext.h"

#include <glm/glm.hpp>

namespace bron::editor {
/// What an overlay is drawn from. Rebuilt by the viewport every frame.
struct OverlayContext {
	EditorContext& editor;

	/// The scene in the viewport: the play copy while playing. Null with no project open.
	Scene* scene;

	/// What the viewport looks through this frame.
	const CameraView& view;

	/// The viewport's size in pixels.
	glm::vec2 target_size;

	bool playing;
};

/// Something the editor draws in the viewport that is not part of the scene: the grid,
/// the selection outline, gizmos, labels. Drawn from editor state every frame and never
/// stored anywhere, so there is nothing to save, copy into play mode or keep out of an
/// export. See notes/editor-overlays.md.
///
/// An overlay reads the scene; it never changes it.
class ViewportOverlay {
public:
	virtual ~ViewportOverlay() = default;

	/// After the world, before the scene's screen-space UI, through the viewport's camera.
	/// Depth testing is on, so the world hides what is behind it.
	virtual void DrawWorld(const OverlayContext& context) {}

	/// Over everything, inside an R2D scene in pixels over the viewport - (0, 0) is its
	/// bottom-left corner. Draw with R2D only; the viewport opens and closes the batch.
	virtual void DrawScreen(const OverlayContext& context) {}
};
} // namespace bron::editor
