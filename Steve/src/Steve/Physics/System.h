#ifndef __PHYSICS_SYSTEM_HEADER__
#define __PHYSICS_SYSTEM_HEADER__

#include "Steve/Core/Core.h"
#include "Steve/Core/Profiling.h"
#include "Steve/Core/Logger.h"

#include "Defs.h"

#include <vector>
#include "Components.h"
#include "BVH.h"

namespace Steve {

    class System {
    public:
        void add(Pt<RigidBody> object);
        void loop();

    private:
        BVH bvh;
    };
}

#endif //__PHYSICS_SYSTEM_HEADER__