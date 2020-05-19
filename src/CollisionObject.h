#ifndef COLLISIONOBJECT_H_
#define COLLISIONOBJECT_H_

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "utils.h"

class World;

class CollisionObject
{
public:
    CollisionObject(World* worldPtr, btCollisionShape* shapePtr) : world(worldPtr), myShape(shapePtr) {}
    virtual ~CollisionObject() noexcept {};

    virtual btCollisionObject* getRawBtCollisionObjPtr() = 0;

    glm::vec3 getPosition()
    {
        btVector3 v = getRawBtCollisionObjPtr()->getWorldTransform().getOrigin();
        return glm::vec3(v.x(), v.y(), v.z());
    }

    void setPositionNoRotation(glm::vec3 position)
    {
        static btTransform transform;
        static btVector3 btPosition;
        btPosition = toBtVec3(position);
        transform.setIdentity();
        transform.setOrigin(btPosition);
        getRawBtCollisionObjPtr()->setWorldTransform(transform);
    }

    void setUserPtr(void* ptr)
    {
        userPtr = ptr;
    }
    void* getUserPtr()
    {
        return userPtr;
    }

protected:
    // Should be called by constructors of derived classes
    // when getRawBtCollisionObjPtr() is ready to return the pointer
    void setBtUserPtr()
    {
        getRawBtCollisionObjPtr()->setUserPointer(this);
    }

    World* world;
    btCollisionShape* myShape;
    void* userPtr;
};

#endif