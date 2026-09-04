#pragma once

#include "Panels/Panel.h"

namespace Bron::Editor
{
	/// Frame time and renderer counters.
	class StatisticsPanel final : public Panel
	{
	public:
		explicit StatisticsPanel(EditorContext& context) : Panel(context) {}

		void OnImGuiRender() override;
	};
}
