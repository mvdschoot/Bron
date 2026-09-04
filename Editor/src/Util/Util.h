#pragma once

#define BR_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define BR_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define BR_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

namespace bron::editor {

}