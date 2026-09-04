#include "ComponentRegistry.h"

#include <cstring>

#include "imgui.h"

namespace bron::editor
{
	using namespace ImGui;

	namespace
	{
		// ----------------------------------------------------------------
		// Draw functions - the only part written per component type
		// ----------------------------------------------------------------

		void DrawTag(Scene& scene, const entt::entity entity)
		{
			TagComponent& tag = scene.reg.get<TagComponent>(entity);

			char buffer[256];
			std::strncpy(buffer, tag.name.c_str(), sizeof(buffer) - 1);
			buffer[sizeof(buffer) - 1] = '\0';

			if (InputText("Name", buffer, sizeof(buffer)))
				tag.name = buffer;
		}

		// Converting a quaternion to euler angles is lossy, so the result is cached per entity:
		// recomputing it every frame makes a rotation drag jump around near the poles.
		entt::entity sEulerCacheOwner = entt::null;
		glm::vec3 sEulerCache{0.0f};

		void DrawTransform(Scene& scene, const entt::entity entity)
		{
			TransformComponent& t = scene.reg.get<TransformComponent>(entity);

			DragFloat3("Position", value_ptr(t.Position));

			if (sEulerCacheOwner != entity)
			{
				sEulerCacheOwner = entity;
				sEulerCache = glm::degrees(glm::eulerAngles(t.RotationQuat));
			}

			if (DragFloat3("Rotation", value_ptr(sEulerCache)))
				t.RotationQuat = glm::quat(glm::radians(sEulerCache));

			DragFloat3("Scaling", value_ptr(t.Scaling));
		}

		void DrawHierarchy(Scene& scene, const entt::entity entity)
		{
			const HierarchyComponent& h = scene.reg.get<HierarchyComponent>(entity);

			if (h.parent == entt::null)
				TextDisabled("Parent: none");
			else
				Text("Parent: %s", scene.reg.get<TagComponent>(h.parent).name.c_str());

			Text("Children: %d", static_cast<int>(h.children.size()));
		}

		void DrawMesh(Scene& scene, const entt::entity entity)
		{
			const MeshComponent& mesh = scene.reg.get<MeshComponent>(entity);

			Text("Vertices: %d", static_cast<int>(mesh.vertexData.positions.size()));
			Text("Indices: %d", static_cast<int>(mesh.vertexData.indices.size()));
			Text("Shader: %s", mesh.material ? mesh.material->shaderName : "none");
			TextDisabled(mesh.vao ? "Uploaded to the GPU" : "Not yet uploaded");
		}

		void DrawPointLight(Scene& scene, const entt::entity entity)
		{
			ColorEdit3("Color", value_ptr(scene.reg.get<PointLightComponent>(entity).color));
		}


		// ----------------------------------------------------------------
		// Registration
		// ----------------------------------------------------------------

		/// Fills in has/add/remove generically; only 'draw' is ever written by hand.
		template<typename T>
		void Register(std::vector<ComponentMeta>& out, const char* name, void (*draw)(Scene&, entt::entity),
					  const u32 flags = ComponentFlags_Default)
		{
			out.push_back({
				name,
				[](Scene& scene, const entt::entity e) { return scene.reg.all_of<T>(e); },
				draw,
				flags & ComponentFlags_Addable
					? +[](Scene& scene, const entt::entity e) { scene.reg.emplace<T>(e); }
					: nullptr,
				flags & ComponentFlags_Removable
					? +[](Scene& scene, const entt::entity e) { scene.reg.remove<T>(e); }
					: nullptr
			});
		}

		std::vector<ComponentMeta> Build()
		{
			std::vector<ComponentMeta> components;

			// Tag, Transform and Hierarchy are attached by Scene::CreateEntity and assumed everywhere,
			// so they are shown but can neither be added nor removed.
			Register<TagComponent>(components, "Tag", DrawTag, ComponentFlags_None);
			Register<TransformComponent>(components, "Transform", DrawTransform, ComponentFlags_None);
			Register<HierarchyComponent>(components, "Hierarchy", DrawHierarchy, ComponentFlags_None);

			// A mesh without vertices or a material cannot be drawn, so it is built by a loader or a
			// factory rather than added from the menu.
			Register<MeshComponent>(components, "Mesh", DrawMesh, ComponentFlags_Removable);

			Register<PointLightComponent>(components, "Light", DrawPointLight);

			return components;
		}
	}

	const std::vector<ComponentMeta>& component_registry::All()
	{
		static const std::vector<ComponentMeta> components = Build();
		return components;
	}

	void component_registry::InvalidateEulerCache()
	{
		sEulerCacheOwner = entt::null;
	}
}
