#include "Core/Icons.h"

#include <array>

#include "Bron.h"

#include "EditorIcons.h" // Generated: see bron_embed_resources() in Editor/CMakeLists.txt.

namespace bron::editor::icons {
namespace {
using Textures = std::array<Ref<bron::Texture>, static_cast<std::size_t>(Id::kCount)>;

Textures& Set() {
	static Textures textures;
	return textures;
}

std::size_t Index(const Id id) { return static_cast<std::size_t>(id); }

/// A switch rather than a table, so adding an Id without adding its art fails to compile
/// instead of drawing whatever happened to sit at that index.
const embedded::Resource& SourceFor(const Id id) {
	switch (id) {
		case Id::kAdd:
			return embedded::kAdd;
		case Id::kDelete:
			return embedded::kDelete;
		case Id::kSave:
			return embedded::kSave;
		case Id::kUp:
			return embedded::kUp;
		case Id::kFolder:
			return embedded::kFolder;
		case Id::kFile:
			return embedded::kFile;
		case Id::kModel:
			return embedded::kModel;
		case Id::kScene:
			return embedded::kScene;
		case Id::kProject:
			return embedded::kProject;
		case Id::kPlay:
			return embedded::kPlay;
		case Id::kPause:
			return embedded::kPause;
		case Id::kStop:
			return embedded::kStop;

		case Id::kCount:
			break;
	}

	BR_APP_ASSERT(false, "No art for icon {}", Index(id));
	return embedded::kFile;
}

/// Names the button to ImGui and, with "##" in front of it, stays out of the label.
const char* NameFor(const Id id) {
	switch (id) {
		case Id::kAdd:
			return "##icon.add";
		case Id::kDelete:
			return "##icon.delete";
		case Id::kSave:
			return "##icon.save";
		case Id::kUp:
			return "##icon.up";
		case Id::kFolder:
			return "##icon.folder";
		case Id::kFile:
			return "##icon.file";
		case Id::kModel:
			return "##icon.model";
		case Id::kScene:
			return "##icon.scene";
		case Id::kProject:
			return "##icon.project";
		case Id::kPlay:
			return "##icon.play";
		case Id::kPause:
			return "##icon.pause";
		case Id::kStop:
			return "##icon.stop";

		case Id::kCount:
			break;
	}

	return "##icon.unknown";
}

/// The engine loads every image bottom up, because that is what a 3D texture coordinate
/// wants; ImGui reads top down. Flipping v here is the same correction the viewport
/// makes for its framebuffer.
constexpr ImVec2 kUv0{0.0f, 1.0f};
constexpr ImVec2 kUv1{1.0f, 0.0f};

ImTextureRef Handle(const Id id) {
	const Ref<bron::Texture>& texture = Set()[Index(id)];
	BR_APP_ASSERT(texture, "icons::Init() has not run");

	return ImTextureRef(static_cast<ImTextureID>(texture->GetId()));
}

/// Icons follow the text colour, which is what makes one theme's set work for the next.
/// GetStyleColorVec4 folds in the global alpha, so an icon inside BeginDisabled() fades
/// the way the label beside it does.
ImVec4 Tint() { return ImGui::GetStyleColorVec4(ImGuiCol_Text); }

void DrawTooltip(const char* tooltip) {
	if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		ImGui::SetTooltip("%s", tooltip);
}
} // namespace

void Init() {
	Textures& textures = Set();

	for (std::size_t i = 0; i < textures.size(); ++i) {
		const embedded::Resource& source = SourceFor(static_cast<Id>(i));
		textures[i] = Texture2D::CreateFromMemory(source.data, static_cast<u32>(source.size));
	}

	BR_APP_INFO("Loaded {} editor icons.", textures.size());
}

void Shutdown() {
	for (Ref<bron::Texture>& texture: Set())
		texture.reset();
}

Ref<bron::Texture> Get(const Id id) { return Set()[Index(id)]; }

// ImGui::Image() lost its tint parameter in 1.91.9; ImageWithBg() is where it went, and
// a transparent background makes it the plain Image() it used to be.
void Draw(const Id id) { ImGui::ImageWithBg(Handle(id), Size(), kUv0, kUv1, ImVec4(0, 0, 0, 0), Tint()); }

bool Button(const Id id, const char* tooltip, const ImVec2 size) {
	const bool pressed = ImGui::ImageButton(NameFor(id), Handle(id), size, kUv0, kUv1, ImVec4(0, 0, 0, 0), Tint());
	DrawTooltip(tooltip);

	return pressed;
}

bool SmallButton(const Id id, const char* tooltip) {
	// ImGui has no ImageButton counterpart to SmallButton(), so this is what SmallButton
	// itself does: the same widget with the vertical padding taken out.
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, 0.0f));
	const bool pressed = Button(id, tooltip);
	ImGui::PopStyleVar();

	return pressed;
}
} // namespace bron::editor::icons
