#ifndef CONCATCCALLBACK_H_
#define CONCATCCALLBACK_H_

#include <functional>

#include <btBulletDynamicsCommon.h>

class CollisionObject;

class ContactCallback : public btCollisionWorld::ContactResultCallback
{
public:
    ContactCallback(std::function<void(CollisionObject* first, CollisionObject* second)> callBack, float maxDistance = 0) : callBack(std::move(callBack)), maxDistance(maxDistance) {}

    btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
        const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1) override
    {
        if (cp.getDistance() <= maxDistance)
            callBack(static_cast<CollisionObject*>(const_cast<btCollisionObject*>(colObj0Wrap->getCollisionObject())->getUserPointer()),
                static_cast<CollisionObject*>(const_cast<btCollisionObject*>(colObj1Wrap->getCollisionObject())->getUserPointer()));
        return 0;
    };

private:
    std::function<void(CollisionObject* first, CollisionObject* second)> callBack;
    float maxDistance;
};

#endif