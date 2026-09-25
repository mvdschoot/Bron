#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Input/Event.h"
#include "Bron/Core/Timestep.h"

namespace bron {
/// One slice of the application that wants a say in the frame: something to simulate and
/// draw, a chance at the input, and debug UI of its own. What a layer is for is ordering
/// - which of several independent things gets the frame, and the click, and in what order
/// relative to the rest. See LayerStack for what that order is.
///
/// A layer is a mode, not a widget: a game, a pause menu, a loading screen. Things that
/// are on screen at once and do not take input from one another belong inside one layer,
/// not in several (the editor's panels are the example - EditorLayer owns them).
///
/// None of these are required. A layer overrides the ones it has something to do in.
class BR_API Layer {
public:
	virtual ~Layer() = default;

	/// Called when the layer enters the stack. The window and its graphics context are
	/// up by then, so this is the earliest point at which GPU resources can be made.
	virtual void OnAttach() {}

	/// Called when it leaves, while the graphics context is still alive. Release
	/// anything held on the GPU here.
	virtual void OnDetach() {}

	/// Marking the event handled stops it reaching the layers underneath.
	virtual void OnEvent(Event& event) {}

	virtual void OnUpdate(Timestep ts) {}
	virtual void OnImGuiRender() {}
};
} // namespace bron
