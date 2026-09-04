#pragma once

#include "Bron/Core/Core.h"

namespace bron {

// The physics code is written against a single scalar type so it can be swapped
// for double without touching every formula.
using real = float;

} // namespace bron
