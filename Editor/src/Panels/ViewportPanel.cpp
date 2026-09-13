#include "Panels/ViewportPanel.h"

#include "Panels/ComponentRegistry.h"

#include <ImGuizmo.h>
#include <glm/gtx/matrix_decompose.hpp>

namespace bron::editor {
namespace {
// What is selected is not always what is drawn: a loaded model is a parent entity whose
// meshes hang off it as children, and only the children carry geometry. Outlining a
// selection means outlining every mesh underneath it.
void CollectMeshes(Scene& scene, const entt::entity entity, std::vector<entt::entity>& out) {
	if (entity == entt::null)
		return;

	if (scene.reg.all_of<MeshComponent>(entity))
		out.push_back(entity);

	if (const HierarchyComponent* hierarchy = scene.reg.try_get<HierarchyComponent>(entity)) {
		for (const entt::entity child: hierarchy->children)
			CollectMeshes(scene, child, out);
	}
}
} // namespace

void ViewportPanel::OnAttach() {
	spec_.width = Application::GetWindow()->GetWindowWidth();
	spec_.height = Application::GetWindow()->GetWindowHeight();
	spec_.attachments = {FramebufferTextureFormat::kRgba8, FramebufferTextureFormat::kRedInteger,
						 FramebufferTextureFormat::kDepth24Stencil8};

	framebuffer_ = Framebuffer::Create(spec_);
	framebuffer_->Unbind();

	viewport_size_ = {static_cast<float>(spec_.width), static_cast<float>(spec_.height)};
}

void ViewportPanel::OnUpdate(const Timestep ts) {
	// WASD orbit is held-key state scaled by the timestep, so it is polled here rather than
	// driven by events; the discrete shortcuts live in OnKeyPressed instead.
	//
	// OnUpdate runs before ImGui::NewFrame(), so there is no current window to ask here -
	// the flag is what the panel saw last frame.
	if (focused_)
		context_.camera.OnUpdate(ts);

	framebuffer_->Bind();
	Command::Clear();

	// Has to come after the clears: glClear covers every enabled draw buffer, so clearing
	// the id attachment first would just be overwritten. -1 is the "nothing here" value,
	// since entity 0 is a perfectly valid entity.
	framebuffer_->ClearAttachmentInt(1, -1);

	Command::EnableBlend();
	GridRenderer::Draw();

	Command::EnableDepth();
	if (context_.HasScene()) {
		SceneRenderer::Draw(*context_.active_scene);

		std::vector<entt::entity> selected_meshes;
		CollectMeshes(*context_.active_scene, context_.selection, selected_meshes);
		SceneRenderer::DrawOutline(*context_.active_scene, selected_meshes);
	}

	framebuffer_->Unbind();
}

void ViewportPanel::Resize(const ImVec2 size) {
	// Minimising the application collapses the panel to 0x0. Resizing the framebuffer to
	// that gives it 0x0 attachments, which are incomplete - so hold on to the last good
	// size and pick the resize back up when the window is restored.
	if (size.x <= 0.0f || size.y <= 0.0f)
		return;

	if (CompareFloat(size.x, viewport_size_.x) && CompareFloat(size.y, viewport_size_.y))
		return;

	viewport_size_ = size;
	viewport_position_ = ImGui::GetCursorScreenPos();

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

	focused_ = ImGui::IsWindowFocused();
	hovered_ = ImGui::IsWindowHovered();

	// The position of these 2 lines in this function is very important,
	// It sets the viewport position and size, which changes based on location in this function.
	const ImVec2 available = ImGui::GetContentRegionAvail();
	Resize(available);

	const uint64_t texture_id = framebuffer_->GetColorAttachId(0);
	ImGui::Image(texture_id, viewport_size_, ImVec2{0, 1}, ImVec2{1, 0});

	DrawGizmo();

	ImGui::End();
}

// Reports the entity drawn under the cursor, or -1 for empty space. The id attachment is
// written by the scene pass, so this reads what was rendered last frame.
entt::entity ViewportPanel::ReadHoveredEntity() const {
	if (!hovered_)
		return entt::null;

	const ImVec2 mouse = ImGui::GetMousePos();

	const int x = static_cast<int>(mouse.x - viewport_position_.x);
	// ImGui counts down from the top, OpenGL counts up from the bottom.
	const int y = static_cast<int>(viewport_size_.y - (mouse.y - viewport_position_.y));

	// Hovering the panel is not the same as hovering the image - the cursor can be over
	// the title bar or the scrollbar, and a read outside the attachment is undefined.
	if (x < 0 || y < 0 || x >= static_cast<int>(viewport_size_.x) || y >= static_cast<int>(viewport_size_.y))
		return entt::null;

	const int entity_id = framebuffer_->ReadPixelInt(1, x, y);
	return static_cast<entt::entity>(entity_id);
}

void ViewportPanel::OnEvent(Event& event) {
	EventDispatcher dispatcher(event);

	// Scrolling follows the cursor, the way it does everywhere else; typed shortcuts
	// need the panel to actually have focus.
	if (hovered_)
		dispatcher.Dispatch<MouseScrolledEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnMouseScrolled));
	dispatcher.Dispatch<MouseButtonPressedEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnMouseClicked));

	if (focused_)
		dispatcher.Dispatch<KeyPressedEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnKeyPressed));
}

bool ViewportPanel::OnKeyPressed(KeyPressedEvent& event) const {
	switch (event.GetKey()) {
		case key::T:
			context_.gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
			return true;
		case key::R:
			context_.gizmo_operation = ImGuizmo::OPERATION::ROTATE;
			return true;
		case key::H:
			context_.gizmo_operation = ImGuizmo::OPERATION::SCALE;
			return true;
		case key::F:
			// Framing an empty selection has nothing to aim at, so leave the key unhandled.
			if (!context_.HasSelection())
				return false;

			context_.camera.Focus(context_.active_scene->reg.get<TransformComponent>(context_.selection).Position);
			return true;
		default:
			return false;
	}
}

bool ViewportPanel::OnMouseScrolled(MouseScrolledEvent& event) const { return context_.camera.OnMouseScrolled(event); }

bool ViewportPanel::OnMouseClicked(MouseButtonPressedEvent& event) const {
	if (hovered_) {
		entt::entity entity = ReadHoveredEntity();
		context_.selection = entity;
		return true;
	}
	return false;
}

void ViewportPanel::DrawGizmo() const {
	if (!context_.HasSelection())
		return;

	Scene& scene = *context_.active_scene;
	const entt::entity selected = context_.selection;

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	const ImVec2 viewport_min_region = ImGui::GetWindowContentRegionMin();
	const ImVec2 viewport_offset = ImGui::GetWindowPos();
	ImGuizmo::SetRect(viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y,
					  viewport_size_.x, viewport_size_.y);

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
