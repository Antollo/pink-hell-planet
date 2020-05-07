
#include "RigidBody.h"

RigidBody::RigidBody(World& w, btCollisionShape* shape, float mass, glm::vec3 position)
    : world(w), myShape(shape)
{
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(toBtVec3(position));
    btScalar btmass(mass);
    btVector3 localInertia(0.f, 0.f, 0.f);
    if (mass > 0)
        shape->calculateLocalInertia(btmass, localInertia);
    motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(btmass, motionState, shape, localInertia);
    body = new btRigidBody(rbInfo);

    world.dynamicsWorld->addRigidBody(body);
}