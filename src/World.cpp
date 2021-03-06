
#include "World.h"

#include "RigidBody.h"
#include "ContactCallback.h"
#include "GhostObject.h"

#include <algorithm>

World::World()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0.f, g, 0.f));
}

std::vector<CollisionObject*> World::getColliding(CollisionObject& body) const
{
    std::vector<CollisionObject*> res;
    ContactCallback cb([&res, &body](CollisionObject* first, CollisionObject* second){
        if (first != &body)
            res.push_back(first);
        if (second != &body)
            res.push_back(second);
    });
    dynamicsWorld->contactTest(body.getRawBtCollisionObjPtr(), cb);
    std::sort(res.begin(), res.end());
    res.erase(std::unique(res.begin(), res.end()), res.end());
    return res;
}

bool World::areColliding(CollisionObject& a, CollisionObject& b) const
{
    bool res = false;
    ContactCallback cb([&res](CollisionObject* first, CollisionObject* second) {
        res = true;
    });
    dynamicsWorld->contactPairTest(a.getRawBtCollisionObjPtr(), b.getRawBtCollisionObjPtr(), cb);
    return res;
}

bool World::pointInShape(CollisionObject& object, glm::vec3 point) const
{
    static btSphereShape sphere(0.f);
    static GhostObject colObj(nullptr, &sphere, point);
    colObj.setPositionNoRotation(point);
    return areColliding(colObj, object);
}
