#pragma once

#include "Bron.h"

#include "Core/EditorContext.h"

namespace bron::editor {
/// One dockable editor window. The editor layer owns the panels, forwards the frame to
/// them in order and knows nothing else about them.
class Panel {
public:
	explicit Panel(EditorContext& context) : context_(context) {}
	virtual ~Panel() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnEvent(Event& event) {}

	/// Work that has to happen before any ImGui window is submitted, such as rendering
	/// the scene into a texture the panel then displays.
	virtual void OnUpdate(Timestep ts) {}

	virtual void OnImGuiRender() = 0;

protected:
	EditorContext& context_;
};
} // namespace bron::editor
