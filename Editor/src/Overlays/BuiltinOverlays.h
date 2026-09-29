#pragma once

#include "Overlays/ViewportOverlay.h"

namespace bron::editor {
/// The ground grid, while editing.
class GridOverlay final : public ViewportOverlay {
public:
	void DrawWorld(const OverlayContext& context) override;
};

/// An outline around every mesh at or below the selection, while editing.
class SelectionOutlineOverlay final : public ViewportOverlay {
public:
	void DrawWorld(const OverlayContext& context) override;
};

/// A line of text in the corner, for trying out 2D drawing in the viewport.
class DebugTextOverlay final : public ViewportOverlay {
public:
	void DrawScreen(const OverlayContext& context) override;
};
} // namespace bron::editor
