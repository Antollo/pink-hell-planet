#ifndef WORLD_H_
#define WORLD_H_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>

#include "CollisionObject.h"
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
        //delete groundShape;
    }

    void update(float delta)
    {
        dynamicsWorld->stepSimulation(delta, 10);
    }

    std::vector<CollisionObject*> getColliding(CollisionObject& body);

    bool areColliding(CollisionObject& a, CollisionObject& b);
    bool pointInShape(CollisionObject& object, glm::vec3 point);

private:
    friend class RigidBody;
    friend class GhostObject;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btBroadphaseInterface *overlappingPairCache;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    RigidBody* ground;
    btCollisionShape *groundShape;
};

#endif /* !WORLD_H_ */
