#pragma once

#include "Bron/Core/Core.h"
#include "Bron/Core/Profiling.h"
#include "Bron/Core/Logger.h"

#include "Defs.h"

#include <vector>
#include "Components.h"
#include "BVH.h"

namespace bron {

    class System {
    public:
        void add(Ref<RigidBody> object);
        void loop();

    private:
        BVH bvh;
    };
}
