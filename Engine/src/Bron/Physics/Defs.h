#pragma once

namespace bron {

    #define real float

    #include "Bron/Core/Core.h"
    template<typename T>
    using Pt = Ref<T>;

    #define createPt CreateRef
}
