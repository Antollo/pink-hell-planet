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
    ~GhostObject();

    btCollisionObject* getRawBtCollisionObjPtr() override
    {
        return ghost;
    }

protected:
    btGhostObject* ghost;
};

#endif