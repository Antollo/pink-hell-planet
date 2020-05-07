#ifndef PHYSICSOBJECT_H_
#define PHYSICSOBJECT_H_

#include <memory>
#include "DrawableObject.h"
#include "RigidBody.h"
#include "World.h"

class PhysicsObject : public DrawableObject, public RigidBody
{
public:
    PhysicsObject(World &w, btCollisionShape *shape, float mass, glm::vec3 position) : RigidBody(w, shape, mass, position) {}
    /*
    PhysicsObject(World &w, btCollisionShape *shape) : world(w), myShape(shape)
    {
        btTransform startTransform;
        startTransform.setIdentity();
        btScalar mass(4.f);
        btVector3 localInertia(0.f, 1.f, 1.f);
        shape->calculateLocalInertia(mass, localInertia);
        startTransform.setOrigin(btVector3(0, 5, 0));
        motionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
        body = new btRigidBody(rbInfo);

        world.dynamicsWorld->addRigidBody(body);
    }
    */
    /*
    ~PhysicsObject()
    {
        delete motionState;
        delete body;
    }
    */
    void update(float delta) override
    {
        btTransform m;
        body->getMotionState()->getWorldTransform(m);
        m.getOpenGLMatrix(glm::value_ptr(M));
    }
    /*
    glm::vec3 getPosition() const
    {
        btVector3 v = body->getWorldTransform().getOrigin();
        return glm::vec3(v.x(), v.y(), v.z());
    }
    */

protected:
    /*
    static std::unique_ptr<btCollisionShape> shapeFromVertices(const std::vector<float> &vertices)
    {
        btConvexHullShape *shape = new btConvexHullShape(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
        shape->optimizeConvexHull();
        return std::unique_ptr<btCollisionShape>(shape);
    }
    static std::unique_ptr<btCollisionShape> shapeFromVertices(std::vector<float> &&vertices)
    {
        btConvexHullShape *shape = new btConvexHullShape(vertices.data(), vertices.size() / 3, 3 * sizeof(float));
        shape->optimizeConvexHull();
        return std::unique_ptr<btCollisionShape>(shape);
    }

    btRigidBody *body;
    */

private:
    /*
    btCollisionShape *myShape;
    btDefaultMotionState *motionState;

    World &world;
    */
};
#endif /* !PHYSICSOBJECT_H_ */
