#pragma once

#include "Bron.h"

#include "Layers/WorldLayer.h"

namespace bron::runtime {
/// A shipped game. It exists to stand one layer up; what that layer runs comes from the
/// manifest the exporter wrote beside this executable.
class App final : public Application {
public:
	App();

private:
	// Owned here, not by the layer stack - Application's stack only orders what it is
	// given. It outlives the stack, which is detached at the end of Application::Run().
	WorldLayer world_layer_;
};
} // namespace bron::runtime
