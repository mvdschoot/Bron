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

private:
	/// Keeps the framebuffer and the camera projection matching the panel.
	void Resize(ImVec2 size);
	void DrawGizmo();

	Ref<Framebuffer> framebuffer_;
	FramebufferSpecification spec_;

	ImVec2 size_{0.0f, 0.0f};
};
} // namespace bron::editor
