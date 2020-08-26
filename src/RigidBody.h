#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include <memory>
#include <vector>

#include "CollisionObject.h"
#include "utils.h"
#include "World.h"
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <glm/gtx/vector_angle.hpp>


class RigidBody : public CollisionObject
{
public:
    RigidBody(World *w, btCollisionShape *shape, float mass, const glm::vec3 &position);
    ~RigidBody();

    btCollisionObject *getRawBtCollisionObjPtr() override
    {
        return body;
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

    void setPosition(const glm::vec3 &position)
    {
        btTransform transform = body->getWorldTransform();
        transform.setOrigin(toBtVec3(position));
        body->setWorldTransform(transform);
        body->getMotionState()->setWorldTransform(transform);

        body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        body->clearForces();
    }

    void setTransform(const btTransform &transform)
    {
        body->setWorldTransform(transform);
        body->getMotionState()->setWorldTransform(transform);

        body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
        body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
        body->clearForces();
    }

    void applyCentralImpulse(const glm::vec3 &v)
    {
        body->applyCentralImpulse(toBtVec3(v));
    }

protected:
    btRigidBody *body;

private:
    btDefaultMotionState *motionState;
};

#endif
