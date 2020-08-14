#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include <memory>
#include <vector>

#include "CollisionObject.h"
#include "utils.h"
#include "World.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>

class World;

class RigidBody : public CollisionObject
{
public:
    RigidBody(World* w, btCollisionShape* shape, float mass, glm::vec3 position);
    ~RigidBody();

    btCollisionObject* getRawBtCollisionObjPtr() override
    {
        return body;
    }

protected:
    btRigidBody* body;

private:
    btDefaultMotionState* motionState;
};

#endif
