#include "Panels/ViewportPanel.h"

#include "Panels/ComponentRegistry.h"

#include "Bron/Scene/AssetManager.h"

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

	if (scene.reg.all_of<MeshMaterialComponent>(entity))
		out.push_back(entity);

	if (const HierarchyComponent* hierarchy = scene.reg.try_get<HierarchyComponent>(entity)) {
		for (const entt::entity child: hierarchy->children)
			CollectMeshes(scene, child, out);
	}
}

// Grows [min, max] by the bounds of every mesh at or below 'entity'. 'to_space' takes the
// entity's local space into the space the bounds are gathered in.
void AccumulateBounds(Scene& scene, const entt::entity entity, const glm::mat4& to_space, glm::vec3& min,
					  glm::vec3& max, bool& found) {
	if (const MeshMaterialComponent* component = scene.reg.try_get<MeshMaterialComponent>(entity)) {
		const Ref<assets::MeshAsset> mesh = assets::AssetManager::Instance().Get<assets::MeshAsset>(component->mesh);

		if (mesh && !mesh->mesh_data.positions.empty()) {
			// All eight corners, since a rotation can make any of them the extreme one.
			for (int corner = 0; corner < 8; corner++) {
				const glm::vec3 local(corner & 1 ? mesh->aabb.max.x : mesh->aabb.min.x,
									  corner & 2 ? mesh->aabb.max.y : mesh->aabb.min.y,
									  corner & 4 ? mesh->aabb.max.z : mesh->aabb.min.z);
				const glm::vec3 point = glm::vec3(to_space * glm::vec4(local, 1.0f));

				min = glm::min(min, point);
				max = glm::max(max, point);
			}
			found = true;
		}
	}

	for (const entt::entity child: scene.reg.get<HierarchyComponent>(entity).children)
		AccumulateBounds(scene, child, to_space * *scene.reg.get<TransformComponent>(child), min, max, found);
}

// The centre of what the entity draws - its own mesh and every mesh below it - in the
// entity's local space. Meshes keep their vertices where the model file put them, so an
// entity's origin can sit well away from its geometry; this is where a user expects to
// grab it. An entity that draws nothing uses its origin.
glm::vec3 LocalPivot(Scene& scene, const entt::entity entity) {
	glm::vec3 min(std::numeric_limits<float>::max());
	glm::vec3 max(std::numeric_limits<float>::lowest());
	bool found = false;

	AccumulateBounds(scene, entity, glm::mat4(1.0f), min, max, found);

	return found ? (min + max) * 0.5f : glm::vec3(0.0f);
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
	// Select the correct camera to use first

	// The aspect comes from the panel, not the camera: the projection has to follow
	// whatever the framebuffer currently is, or the scene is stretched to fit it.
	float aspect_ratio = viewport_size_.y > 0.0f ? viewport_size_.x / viewport_size_.y : 1.0f;
	if (context_.camera_preview == entt::null) {
		context_.camera.OnUpdate(ts);
		view_ = context_.camera.View(aspect_ratio);
	} else {
		CameraComponent& camera_component = context_.active_scene->reg.get<CameraComponent>(context_.camera_preview);
		view_ = ViewFrom(camera_component, context_.active_scene->WorldTransform(context_.camera_preview),
						 aspect_ratio);
	}

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
	GridRenderer::Draw(view_);

	Command::EnableDepth();
	if (context_.HasScene()) {
		SceneRenderer::Draw(*context_.active_scene, view_);

		std::vector<entt::entity> selected_meshes;
		CollectMeshes(*context_.active_scene, context_.selection, selected_meshes);
		SceneRenderer::DrawOutline(*context_.active_scene, view_, selected_meshes);
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
	if (hovered_) {
		dispatcher.Dispatch<MouseScrolledEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnMouseScrolled));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnMouseClicked));
	}

	if (focused_) {
		dispatcher.Dispatch<KeyPressedEvent>(BR_BIND_EVENT_FN(ViewportPanel::OnKeyPressed));
	}
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
		case key::F: {
			// Framing an empty selection has nothing to aim at, so leave the key unhandled.
			if (!context_.HasSelection())
				return false;

			Scene& scene = *context_.active_scene;
			const glm::vec3 pivot = LocalPivot(scene, context_.selection);
			context_.camera.Focus(glm::vec3(scene.WorldTransform(context_.selection) * glm::vec4(pivot, 1.0f)));
			return true;
		}
		default:
			return false;
	}
}

bool ViewportPanel::OnMouseScrolled(MouseScrolledEvent& event) const { return context_.camera.OnMouseScrolled(event); }

bool ViewportPanel::OnMouseClicked(MouseButtonPressedEvent& event) const {
	if (hovered_ && !guizmo_hovered_) {
		entt::entity entity = ReadHoveredEntity();
		context_.selection = entity;
		return true;
	}
	return false;
}

void ViewportPanel::DrawGizmo() {
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

	glm::mat4 proj = view_.projection;
	glm::mat4 view = view_.view;

	// The gizmo manipulates a world transform; the component stores a local one. It is
	// drawn at the centre of the geometry rather than the entity's origin, so moving,
	// rotating and scaling all happen around that point.
	const glm::vec3 pivot = LocalPivot(scene, selected);
	glm::mat4 transform = scene.WorldTransform(selected) * glm::translate(glm::mat4(1.0f), pivot);

	ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), context_.gizmo_operation, ImGuizmo::LOCAL,
						 glm::value_ptr(transform));

	guizmo_hovered_ = ImGuizmo::IsOver();
	if (!ImGuizmo::IsUsing()) // only update if the user is manipulating
		return;

	TransformComponent& comp = scene.reg.get<TransformComponent>(selected);

	// Back out the parent transform, so the entity keeps its place in the hierarchy.
	const entt::entity parent = scene.reg.get<HierarchyComponent>(selected).parent;
	const glm::mat4 parent_transform = parent != entt::null ? scene.WorldTransform(parent) : glm::mat4(1.0f);
	// And the pivot offset, which is not part of the entity's transform.
	const glm::mat4 local = glm::inverse(parent_transform) * transform * glm::translate(glm::mat4(1.0f), -pivot);

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
