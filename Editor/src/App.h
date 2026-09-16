#pragma once

#include "Bron.h"

#include "Core/EditorLayer.h"

namespace bron::editor {
/// The editor executable. It exists to stand one layer up; all of the editor lives there.
class App final : public Application {
public:
	App();
	~App() override;

private:
	// Owned here, not by the layer stack - Application's stack only orders what it is
	// given. It outlives the stack, which is detached at the end of Application::Run().
	EditorLayer editor_layer;
};
} // namespace bron::editor
