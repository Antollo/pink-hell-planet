#ifndef PHYSICSOBJECT_H_
#define PHYSICSOBJECT_H_

#include <memory>
#include "DrawableObject.h"
#include "RigidBody.h"
#include "World.h"

class PhysicsObject : public DrawableObject, public RigidBody
{
public:
    PhysicsObject(World &w, btCollisionShape *shape, float mass, const glm::vec3 &position) : RigidBody(&w, shape, mass, position) {}

    void update(float delta) override
    {
        btTransform m;
        body->getMotionState()->getWorldTransform(m);
        m.getOpenGLMatrix(glm::value_ptr(M));
    }
};

#endif /* !PHYSICSOBJECT_H_ */
