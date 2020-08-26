#ifndef GHOSTOBJECT_H_
#define GHOSTOBJECT_H_

#include <memory>
#include <vector>

#include "CollisionObject.h"
#include "utils.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <glm/glm.hpp>

class World;

class GhostObject : public CollisionObject
{
public:
    GhostObject(World* w, btCollisionShape* shape, glm::vec3 position = glm::vec3(0.f, 0.f, 0.f));
    virtual ~GhostObject();

    btCollisionObject* getRawBtCollisionObjPtr() override
    {
        return ghost;
    }

    void doForColliding(std::function<void(CollisionObject*)> f)
    {
        for (int i = 0; i < ghost->getNumOverlappingObjects(); i++)
        {
            f(reinterpret_cast<CollisionObject*>(ghost->getOverlappingObject(i)->getUserPointer()));
        }
    }

protected:
    btGhostObject* ghost;
};

#endif
