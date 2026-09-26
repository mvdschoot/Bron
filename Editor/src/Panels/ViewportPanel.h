#pragma once

#include "PanelInput.h"
#include "Panels/Panel.h"

namespace bron::editor {
/// Renders the scene into an offscreen framebuffer and shows it as an image, with the
/// transform gizmo drawn on top. Owning the framebuffer here keeps the render target
/// tied to the panel whose size decides it.
class ViewportPanel final : public Panel {
public:
	ViewportPanel(EditorContext& context, const std::string& name, const std::string& display_name) :
		Panel(context, name, display_name) {
		window_flags_ = ImGuiWindowFlags_NoNav;
	}

	void OnAttach() override;
	void OnUpdate(Timestep ts) override;
	void OnEvent(Event& event) override;

	bool OnMouseScrolled(MouseScrolledEvent& event) const;
	bool OnMouseClicked(MouseButtonPressedEvent& event) const;
	bool OnKeyPressed(KeyPressedEvent& event) const;

protected:
	void ImGuiContent() override;

	// The image has to sit flush against the window edge.
	void PushWindowStyle() override { ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); }
	void PopWindowStyle() override { ImGui::PopStyleVar(); }

private:
	/// Keeps the framebuffer matching the panel.
	void Resize(ImVec2 size);
	void DrawGizmo();
	/// Logs the entity under the cursor, read back from the id attachment.
	entt::entity ReadHoveredEntity() const;

	void PlayStopButton() const;

	Ref<Framebuffer> framebuffer_;
	FramebufferSpecification spec_;

	/// What this frame was drawn through, rebuilt at the top of OnUpdate. Kept because
	/// the gizmo needs the same matrices during OnImGuiRender, and recomputing them
	/// there would let the two disagree for a frame.
	CameraView view_;

	/// Exludes the top bar, this is the actual drawable size in pixels.
	ImVec2 viewport_size_{0.0f, 0.0f};

	/// Exludes the top bar, this is where the viewport image actually starts.
	ImVec2 viewport_position_{0.0f, 0.0f};

	bool guizmo_hovered_ = false;

	/// tmp
	assets::AssetHandle font_handle;
};
} // namespace bron::editor
