#pragma once

#include "Bron.h"
#include "PanelInput.h"

#include "Core/EditorContext.h"

namespace bron::editor {
class PanelInput;
/// One dockable editor window. The editor layer owns the panels, forwards the frame to
/// them in order and knows nothing else about them.
class Panel {
public:
	explicit Panel(EditorContext& context, const std::string& name, const std::string& display_name) :
		context_(context), name_(name), display_name_(display_name), panel_input_(PanelInput(this)) {}
	virtual ~Panel() = default;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnEvent(Event& event) {}

	/// Work that has to happen before any ImGui window is submitted, such as rendering
	/// the scene into a texture the panel then displays.
	virtual void OnUpdate(Timestep ts) {}

	/// Not virtual: Begin, the shared work, the content and End always happen in this
	/// order, and only here - so no panel can leave the window stack unbalanced. Panels
	/// vary the window through the fields and hooks below instead.
	void OnImGuiRender() {
		if (closable_ && !open_)
			return;

		PushWindowStyle();
		const bool visible = ImGui::Begin(display_name_.c_str(), closable_ ? &open_ : nullptr, window_flags_);
		PopWindowStyle();

		if (visible) {
			SetFocusHover();
			ImGuiContent();
		}
		ImGui::End();
	}

	void Open() { open_ = true; }

	void SetFocusHover() {
		if (ImGui::IsWindowFocused())
			context_.focused_panel = this;
		if (ImGui::IsWindowHovered())
			context_.hovered_panel = this;
	}

	bool IsFocussed() const { return context_.focused_panel == this; }
	bool IsHovered() const { return context_.hovered_panel == this; }


protected:
	/// What goes inside the window. Begin has already succeeded when this is called, and
	/// End follows it - an early exit is a plain return.
	virtual void ImGuiContent() = 0;

	/// Style that has to be in place when the window is created, such as its padding.
	/// Popped straight after Begin, so it does not leak into the content.
	virtual void PushWindowStyle() {}
	virtual void PopWindowStyle() {}

	/// Set from a subclass constructor when the window differs from the default.
	ImGuiWindowFlags window_flags_ = ImGuiWindowFlags_None;
	/// Gives the window a close button; a closed panel is skipped until Open() is called.
	bool closable_ = false;
	bool open_ = true;

	EditorContext& context_;
	std::string name_;
	std::string display_name_;
	PanelInput panel_input_;
};
} // namespace bron::editor
