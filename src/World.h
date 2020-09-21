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
    }

    void update(float delta)
    {
        dynamicsWorld->stepSimulation(delta, 10);
    }

    std::vector<CollisionObject *> getColliding(CollisionObject &body) const;

    bool areColliding(CollisionObject &a, CollisionObject &b) const;
    bool pointInShape(CollisionObject &object, glm::vec3 point) const;

    std::unique_ptr<btCollisionWorld::AllHitsRayResultCallback> getAllRaycastResults(const glm::vec3 &begin, const glm::vec3 &end) const
    {
        std::unique_ptr<btCollisionWorld::AllHitsRayResultCallback> callback = std::make_unique<btCollisionWorld::AllHitsRayResultCallback>(toBtVec3(begin), toBtVec3(end));
        dynamicsWorld->rayTest(toBtVec3(begin), toBtVec3(end), *callback);
        return callback;
    }

    std::unique_ptr<btCollisionWorld::ClosestRayResultCallback> getRaycastResult(const glm::vec3 &begin, const glm::vec3 &end) const
    {
        std::unique_ptr<btCollisionWorld::ClosestRayResultCallback> callback = std::make_unique<btCollisionWorld::ClosestRayResultCallback>(toBtVec3(begin), toBtVec3(end));
        dynamicsWorld->rayTest(toBtVec3(begin), toBtVec3(end), *callback);
        return callback;
    }

    static constexpr float g = -9.81f;

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
