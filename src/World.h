#ifndef WORLD_H_
#define WORLD_H_

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>

#include "CollisionObject.h"

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

    std::vector<CollisionObject *> getColliding(CollisionObject &body);

    bool areColliding(CollisionObject &a, CollisionObject &b);
    bool pointInShape(CollisionObject &object, glm::vec3 point);

    std::unique_ptr<btCollisionWorld::AllHitsRayResultCallback> getRaycastResults(const glm::vec3 &begin, const glm::vec3 &end)
    {
        std::unique_ptr<btCollisionWorld::AllHitsRayResultCallback> callback = std::make_unique<btCollisionWorld::AllHitsRayResultCallback>(toBtVec3(begin), toBtVec3(end));
        dynamicsWorld->rayTest(toBtVec3(begin), toBtVec3(end), *callback);
        return callback;
    }

private:
    friend class RigidBody;
    friend class GhostObject;
    btDefaultCollisionConfiguration *collisionConfiguration;
    btCollisionDispatcher *dispatcher;
    btBroadphaseInterface *overlappingPairCache;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld *dynamicsWorld;

    RigidBody *ground;
    btCollisionShape *groundShape;
};

#endif /* !WORLD_H_ */
