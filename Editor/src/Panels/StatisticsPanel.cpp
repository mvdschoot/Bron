#include "Panels/StatisticsPanel.h"

namespace bron::editor {
void StatisticsPanel::OnImGuiRender() {
	// Begin() returning false only means the contents are clipped (collapsed, or the
	// whole window minimised) - the window is still on the stack, so End() has to be
	// called either way or the outer dockspace window loses its End().
	if (ImGui::Begin("Statistics")) {
		ImGui::Text("FPS: %f", 1000.0f / context_.frame_time.GetMilliseconds());

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::NewLine();

		if (ImGui::CollapsingHeader("3D scene renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Indent();
			ImGui::Text("Shaders: %d", SceneRenderer::Statistics.Shaders);
			ImGui::Text("Materials: %d", SceneRenderer::Statistics.Materials);
			ImGui::Text("Meshes: %d", SceneRenderer::Statistics.Meshes);
			ImGui::Text("Draw calls: %d", SceneRenderer::Statistics.DrawCalls);
			ImGui::Text("Uniform calls: %d", SceneRenderer::Statistics.UniformCalls);
			ImGui::Unindent();
		}
	}
	ImGui::End();
}
} // namespace bron::editor
