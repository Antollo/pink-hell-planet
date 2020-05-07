#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include <memory>
#include <vector>

#include "utils.h"
#include "World.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>

class World;

class RigidBody
{
public:
    RigidBody(World& w, btCollisionShape* shape, float mass, glm::vec3 position);
    ~RigidBody()
    {
        delete motionState;
        delete body;
    }

    glm::vec3 getPosition() const
    {
        btVector3 v = body->getWorldTransform().getOrigin();
        return glm::vec3(v.x(), v.y(), v.z());
    }

protected:
    static std::unique_ptr<btCollisionShape> shapeFromVertices(const std::vector<float>& vertices)
    {
        btConvexHullShape* shape = new btConvexHullShape(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
        shape->optimizeConvexHull();
        return std::unique_ptr<btCollisionShape>(shape);
    }
    static std::unique_ptr<btCollisionShape> shapeFromVertices(std::vector<float>&& vertices)
    {
        btConvexHullShape* shape = new btConvexHullShape(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
        shape->optimizeConvexHull();
        return std::unique_ptr<btCollisionShape>(shape);
    }

    btRigidBody* body;

private:
    btCollisionShape* myShape;
    btDefaultMotionState* motionState;

    World& world;
};

#endif