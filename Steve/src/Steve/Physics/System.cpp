#include "System.h"
#include "Collisions.h"

namespace Steve {
    void System::add(Pt<RigidBody> object)
    {
        bvh.addObject(object);
    }
    
    void System::loop()
    {
        collides({}, {});
    }
}