#include "System.h"
#include "Collisions.h"

namespace bron {
void System::add(Ref<RigidBody> object) { bvh.AddObject(object); }

void System::loop() { Collides({}, {}); }
} // namespace bron
