#ifndef PHYSICSOBJECT_H_
#define PHYSICSOBJECT_H_

#include <memory>
#include <glm/gtx/vector_angle.hpp>
#include "DrawableObject.h"
#include "RigidBody.h"
#include "World.h"

class PhysicsObject : public DrawableObject, public RigidBody
{
public:
    PhysicsObject(World &w, btCollisionShape *shape, float mass, glm::vec3 position) : RigidBody(&w, shape, mass, position) {}

    void update(float delta) override
    {
        btTransform m;
        body->getMotionState()->getWorldTransform(m);
        m.getOpenGLMatrix(glm::value_ptr(M));
    }

    glm::vec3 getPosition() const
    {
        return toGlmVec3(body->getCenterOfMassPosition());
    }

    float getOrientationY() const
    {
        btQuaternion q = body->getOrientation();
        glm::vec3 v = glm::mat3_cast(glm::angleAxis(q.getAngle(), toGlmVec3(q.getAxis()))) * glm::vec3(1.f, 0.f, 0.f);
        return glm::orientedAngle(glm::vec2(1.f, 0.f), glm::normalize(glm::vec2(v.x, v.z)));
    }
};

#endif /* !PHYSICSOBJECT_H_ */
