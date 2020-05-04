#ifndef PHYSICSOBJECT_H_
#define PHYSICSOBJECT_H_

#include <memory>
#include "DrawableObject.h"
#include "World.h"

class PhysicsObject : public DrawableObject
{
public:
    PhysicsObject(World &w, btCollisionShape *shape) : world(w), myShape(shape)
    {
        btTransform startTransform;
        startTransform.setIdentity();
        btScalar mass(4.f);
        btVector3 localInertia(0, 0.5, 0);
        shape->calculateLocalInertia(mass, localInertia);
        startTransform.setOrigin(btVector3(0, 5, 0));
        myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
        body = new btRigidBody(rbInfo);

        world.dynamicsWorld->addRigidBody(body);
    }
    ~PhysicsObject()
    {
        delete myMotionState;
        delete body;
    }
    void update(double delta) override
    {
        btTransform m;
        body->getMotionState()->getWorldTransform(m);
        m.getOpenGLMatrix(glm::value_ptr(M));
    }

protected:
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
private:
    btCollisionShape *myShape;
    btDefaultMotionState *myMotionState;
    btRigidBody *body;
    World &world;
};
#endif /* !PHYSICSOBJECT_H_ */
