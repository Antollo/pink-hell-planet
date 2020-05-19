
#include "GhostObject.h"
#include "World.h"

GhostObject::GhostObject(World* w, btCollisionShape* shape, glm::vec3 position)
    : CollisionObject(w, shape)
{
    ghost = new btGhostObject();
    ghost->setCollisionShape(shape);
    setBtUserPtr();
    setPositionNoRotation(position);

    if (world != nullptr)
        world->dynamicsWorld->addCollisionObject(ghost);
}

GhostObject::~GhostObject()
{
    if (world != nullptr)
        world->dynamicsWorld->removeCollisionObject(ghost);
    delete ghost;
}
