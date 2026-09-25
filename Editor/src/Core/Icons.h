#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Graphics/Texture.h"

#include "imgui.h"


namespace bron::editor::icons {
/// The editor's own icons. They are not assets: they are part of what the editor looks
/// like, they have to be there on the first frame - before any project is open, so
/// before there is an asset root to resolve anything against - and there is no install
/// tree to ship them in. So they are compiled into the binary, the same argument the
/// engine's shaders are built on (Bron/Graphics/BuiltinShaders.h).
///
/// The art is drawn white on transparent by Editor/resources/icons/generate.py and
/// tinted with the theme's text colour here, so an icon follows the theme instead of
/// needing a light and a dark copy.
enum class Id {
	kAdd,
	kDelete,
	kSave,
	kUp,
	kFolder,
	kFile,
	kModel,
	kScene,
	kProject,
	kPlay,
	kPause,
	kStop,

	kCount
};

/// Square, and the height of a line of text: an icon button then measures the same as a
/// text button beside it, and the whole set follows the interface scale for free.
inline ImVec2 Size() {
	const float side = ImGui::GetFontSize();
	return {side, side};
}


/// Decodes and uploads the set. Needs a live graphics context, so it belongs in a
/// layer's OnAttach rather than in a constructor.
void Init();

/// Drops the textures. Has to run while the context is still up, which is why the set is
/// not simply a function local static: that would free texture names after the window
/// has gone.
void Shutdown();

/// The texture behind an icon, for drawing one by hand. Null before Init().
Ref<bron::Texture> Get(Id id);

/// Draws an icon inline, sized to the current font so it sits on the text baseline of
/// whatever it is next to.
void Draw(Id id);

/// An icon-only button, the same height as a text button. 'tooltip' says what it does -
/// an icon on its own never quite does - and is required for that reason.
///
/// The icon is the button's ImGui id, so two buttons with the same icon in one window
/// need a PushID between them, as anywhere else in ImGui.
bool Button(Id id, const char* tooltip, const ImVec2 size = Size());

/// The same button drawn small, for a header row or a list entry, where a full sized one
/// would set the row height.
bool SmallButton(Id id, const char* tooltip);
} // namespace bron::editor::icons
