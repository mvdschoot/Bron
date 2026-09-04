#include "System.h"
#include "Collisions.h"

namespace bron {
    void System::add(Pt<RigidBody> object)
    {
        bvh.AddObject(object);
    }
    
    void System::loop()
    {
        Collides({}, {});
    }
}