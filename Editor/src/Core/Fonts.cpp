//
// Created by mathi on 28-9-2026.
//
#include "Fonts.h"
#include "EditorFonts.h"
#include "Bron/Scene/AssetManager.h"
#include "Bron/Scene/FontLoader.h"

namespace bron::editor::fonts {

static assets::AssetHandle default_font_handle;

void Init() {
	auto font_resource = embedded::kDefaultFont;
	const std::optional<ImportedFont> imported_font = FontLoader::Import(font_resource.data, font_resource.size);
	BR_APP_ASSERT(imported_font.has_value(), "Default font could not be loaded");

	const Ref<assets::FontAsset> font_asset = assets::AssetManager::ImportFont(*imported_font);
	default_font_handle = assets::AssetManager::Instance().AddMemoryAsset("Default font", assets::kFont, font_asset);
}

assets::AssetHandle GetDefaultFont() { return default_font_handle; }
} // namespace bron::editor::fonts
