#ifndef WORLD_H_
#define WORLD_H_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>

#include "RigidBody.h"

class RigidBody;

class World
{
public:
    World();

    ~World()
    {
        delete dynamicsWorld;
        delete solver;
        delete overlappingPairCache;
        delete dispatcher;
        delete collisionConfiguration;
        delete groundShape;
    }

    void update(float delta)
    {
        dynamicsWorld->stepSimulation(delta, 10);
    }

private:
    friend class RigidBody;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btBroadphaseInterface *overlappingPairCache;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    RigidBody* ground;
    btCollisionShape *groundShape;
};

#endif /* !WORLD_H_ */
