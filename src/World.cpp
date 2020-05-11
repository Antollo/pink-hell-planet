
#include "World.h"

World::World()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    overlappingPairCache = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0.f, -10.f, 0.f));

    groundShape = new btTriangleShape(btVector3(-200, 0, -100), btVector3(200, 0, -100), btVector3(100, 0, 100));
    ground = new RigidBody(*this, groundShape, 0, {0, -6, 0});
    /*
    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -6, 0));
    btScalar mass(0.f);
    btVector3 localInertia(0, 0, 0);
    myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    body = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(body);
    */
}