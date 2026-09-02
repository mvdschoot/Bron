#pragma once

namespace Bron {

    #define real float

    #include "Bron/Core/Core.h"
    template<typename T>
    using Pt = Ref<T>;

    #define createPt createRef
}
