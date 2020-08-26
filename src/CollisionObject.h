#ifndef COLLISIONOBJECT_H_
#define COLLISIONOBJECT_H_

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "utils.h"

class World;

class CollisionObject
{
public:
    static void init();

    CollisionObject(World *worldPtr, btCollisionShape *shapePtr) : world(worldPtr), myShape(shapePtr), ownerPtr(nullptr), id(nextCollisionObjectID++) {}
    virtual ~CollisionObject() noexcept {}

    virtual btCollisionObject *getRawBtCollisionObjPtr() = 0;

    void setPositionNoRotation(glm::vec3 position)
    {
        static btTransform transform;
        static btVector3 btPosition;
        btPosition = toBtVec3(position);
        transform.setIdentity();
        transform.setOrigin(btPosition);
        getRawBtCollisionObjPtr()->setWorldTransform(transform);
    }

    class CollisionObjectOwner
    {
    public:
        virtual ~CollisionObjectOwner() {}
    };

    void setOwnerPtr(CollisionObjectOwner *ptr)
    {
        ownerPtr = ptr;
    }
    CollisionObjectOwner *getOwnerPtr() const
    {
        return ownerPtr;
    }

    virtual void contactAddedCallback(CollisionObject *other) {}
    const World *getWorld() { return world; }

    static void addGlobalMaterialCombinerCallback(std::function<void(btManifoldPoint &, const btCollisionObjectWrapper *, int, int, const btCollisionObjectWrapper *, int, int)> c)
    {
        globalMaterialCombinerCallbacks.push_back(c);
    }

    using idType = unsigned int;
    idType getId() const
    {
        return id;
    }

protected:
    static std::unique_ptr<btCollisionShape> shapeFromVertices(const std::vector<float> &vertices);
    static std::unique_ptr<btCollisionShape> boundingBoxShape(const std::vector<float> &vertices);

    // Should be called by constructors of derived classes
    // when getRawBtCollisionObjPtr() is ready to return the pointer
    void setupBtCollisionObject()
    {
        getRawBtCollisionObjPtr()->setUserPointer(this);
        getRawBtCollisionObjPtr()->setCollisionFlags(getRawBtCollisionObjPtr()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
    }

    World *world;
    btCollisionShape *myShape;
    CollisionObjectOwner *ownerPtr;

private:
    idType id;

    static inline int nextCollisionObjectID = 0;
    static bool customContactAddedCallback(btManifoldPoint &cp, const btCollisionObjectWrapper *colObj0Wrap, int partId0, int index0,
                                           const btCollisionObjectWrapper *colObj1Wrap, int partId1, int index1);
    static inline std::vector<std::function<void(btManifoldPoint&, const btCollisionObjectWrapper*, int, int, const btCollisionObjectWrapper*, int, int)>> globalMaterialCombinerCallbacks;
};

#endif
