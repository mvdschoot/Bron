#pragma once

#include "Panels/Panel.h"

namespace Bron::Editor
{
	/// Renders the scene into an offscreen framebuffer and shows it as an image, with the
	/// transform gizmo drawn on top. Owning the framebuffer here keeps the render target
	/// tied to the panel whose size decides it.
	class ViewportPanel final : public Panel
	{
	public:
		explicit ViewportPanel(EditorContext& context) : Panel(context) {}

		void OnAttach() override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;

	private:
		/// Keeps the framebuffer and the camera projection matching the panel.
		void Resize(ImVec2 size);
		void DrawGizmo();

		Ref<Framebuffer> mFramebuffer;
		FramebufferSpecification mSpec;

		ImVec2 mSize{0.0f, 0.0f};
	};
}
