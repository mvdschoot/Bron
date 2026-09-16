#pragma once

#include "Bron.h"

#include "Core/EditorLayer.h"

namespace bron::editor {
class App final : public Application {
public:
	App();
	~App() override;

private:
	Overlay overlay;
};
} // namespace bron::editor
