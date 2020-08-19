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

class World;

class RigidBody : public CollisionObject
{
public:
    RigidBody(World *w, btCollisionShape *shape, float mass, glm::vec3 position);
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

    auto getRaycastResults(const glm::vec3 &end) const
    {
        auto callback(world->getRaycastResults(getPosition(), end));

        if (callback->m_collisionObjects.size() && static_cast<CollisionObject *>(callback->m_collisionObjects[0]->getUserPointer()) == this)
        {
            callback->m_collisionObjects.removeAtIndex(0);
            callback->m_hitPointWorld.removeAtIndex(0);
            callback->m_hitNormalWorld.removeAtIndex(0);
        }
        return callback;
    }

protected:
    btRigidBody *body;

private:
    btDefaultMotionState *motionState;
};

#endif
