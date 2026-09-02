#include "System.h"
#include "Collisions.h"

namespace Bron {
    void System::add(Pt<RigidBody> object)
    {
        bvh.addObject(object);
    }
    
    void System::loop()
    {
        collides({}, {});
    }
}