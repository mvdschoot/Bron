#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// Renders the scene into an offscreen framebuffer and shows it as an image, with the
/// transform gizmo drawn on top. Owning the framebuffer here keeps the render target
/// tied to the panel whose size decides it.
class ViewportPanel final : public Panel {
public:
	explicit ViewportPanel(EditorContext& context) : Panel(context) {}

	void OnAttach() override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;
	void OnEvent(Event& event) override;

	bool OnMouseScrolled(MouseScrolledEvent& event) const;
	bool OnMouseClicked(MouseButtonPressedEvent& event) const;
	bool OnKeyPressed(KeyPressedEvent& event) const;

private:
	/// Keeps the framebuffer and the camera projection matching the panel.
	void Resize(ImVec2 size);
	void DrawGizmo() const;
	/// Logs the entity under the cursor, read back from the id attachment.
	entt::entity ReadHoveredEntity() const;

	Ref<Framebuffer> framebuffer_;
	FramebufferSpecification spec_;

	/// Exludes the top bar, this is the actual drawable size in pixels.
	ImVec2 viewport_size_{0.0f, 0.0f};

	/// Exludes the top bar, this is where the viewport image actually starts.
	ImVec2 viewport_position_{0.0f, 0.0f};


	/// Whether the panel had focus last frame. OnUpdate runs outside the ImGui frame, so
	/// it cannot query focus itself.
	bool focused_ = false;
	bool hovered_ = false;
};
} // namespace bron::editor
