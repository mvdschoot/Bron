#pragma once

#include "Panels/Panel.h"

namespace bron::editor {
/// Frame time and renderer counters.
class StatisticsPanel final : public Panel {
public:
	StatisticsPanel(EditorContext& context, const std::string& name, const std::string& display_name) :
		Panel(context, name, display_name) {}

	void ImGuiContent() override;
};
} // namespace bron::editor
