#ifndef __PHYSICS_DEFS_HEADER__
#define __PHYSICS_DEFS_HEADER__

namespace Steve {

    #define real float

    #include "Steve/Core/Core.h"
    template<typename T>
    using Pt = Ref<T>;

    #define createPt createRef
}

#endif