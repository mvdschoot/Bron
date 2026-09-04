#include "Panels/ViewportPanel.h"

#include "Panels/ComponentRegistry.h"

#include <ImGuizmo.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace bron::editor {
void ViewportPanel::OnAttach() {
	spec_.width = Application::GetWindow()->GetWindowWidth();
	spec_.height = Application::GetWindow()->GetWindowHeight();

	framebuffer_ = Framebuffer::Create(spec_);
	framebuffer_->Unbind();

	size_ = {static_cast<float>(spec_.width), static_cast<float>(spec_.height)};
}

void ViewportPanel::OnUpdate(const Timestep ts) {
	context_.camera.OnUpdate(ts);

	framebuffer_->Bind();
	Command::Clear();

	Command::EnableBlend();
	GridRenderer::Draw();

	Command::EnableDepth();
	SceneRenderer::Draw(context_.scene);

	framebuffer_->Unbind();
}

void ViewportPanel::Resize(const ImVec2 size) {
	// Minimising the application collapses the panel to 0x0. Resizing the framebuffer to
	// that gives it 0x0 attachments, which are incomplete - so hold on to the last good
	// size and pick the resize back up when the window is restored.
	if (size.x <= 0.0f || size.y <= 0.0f)
		return;

	if (CompareFloat(size.x, size_.x) && CompareFloat(size.y, size_.y))
		return;

	size_ = size;
	spec_.width = static_cast<uint32_t>(size.x);
	spec_.height = static_cast<uint32_t>(size.y);
	framebuffer_->Invalidate();

	// The projection has to follow the panel, otherwise the scene is stretched to fit it.
	context_.camera.SetAspectRatio(size.x / size.y);
}

void ViewportPanel::OnImGuiRender() {
	// The image has to sit flush against the window edge.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	ImGui::PopStyleVar();

	const ImVec2 available = ImGui::GetContentRegionAvail();
	Resize(available);

	const uint64_t texture_id = framebuffer_->GetColorAttachId();
	ImGui::Image(texture_id, available, ImVec2{0, 1}, ImVec2{1, 0});

	DrawGizmo();

	ImGui::End();
}

void ViewportPanel::DrawGizmo() {
	if (!context_.HasSelection())
		return;

	Scene& scene = context_.scene;
	const entt::entity selected = context_.selection;

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	const ImVec2 viewport_min_region = ImGui::GetWindowContentRegionMin();
	const ImVec2 viewport_offset = ImGui::GetWindowPos();
	ImGuizmo::SetRect(viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y, size_.x,
					  size_.y);

	glm::mat4 proj = scene.camera->GetProjectionMatrix();
	glm::mat4 view = scene.camera->GetViewMatrix();

	// The gizmo manipulates a world transform; the component stores a local one.
	glm::mat4 transform = scene.WorldTransform(selected);

	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), context_.gizmo_operation, ImGuizmo::LOCAL,
						 glm::value_ptr(transform));

	if (!ImGuizmo::IsUsing()) // only update if the user is manipulating
		return;

	TransformComponent& comp = scene.reg.get<TransformComponent>(selected);

	// Back out the parent transform, so the entity keeps its place in the hierarchy.
	const entt::entity parent = scene.reg.get<HierarchyComponent>(selected).parent;
	const glm::mat4 parent_transform = parent != entt::null ? scene.WorldTransform(parent) : glm::mat4(1.0f);
	const glm::mat4 local = glm::inverse(parent_transform) * transform;

	// Extract TRS in a stable way
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::quat rotation_quat;
	glm::decompose(local, comp.Scaling, rotation_quat, comp.Position, skew, perspective);

	glm::quat new_quat = glm::normalize(rotation_quat);
	if (glm::dot(rotation_quat, comp.RotationQuat) < 0.0f)
		new_quat = -new_quat;

	comp.RotationQuat = new_quat;

	// The properties panel caches euler angles; the gizmo just changed the quaternion under it.
	component_registry::InvalidateEulerCache();
}
} // namespace bron::editor
