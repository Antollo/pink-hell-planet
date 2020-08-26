#ifndef BULLET_H_
#define BULLET_H_

#include <algorithm>

#include "GhostObject.h"
#include "PhysicsObject.h"
#include "PlayableObject.h"

class Bullet : public PhysicsObject
{
public:
    static void init()
    {
        shape = PhysicsObject::shapeFromVertices(vertexArray.load("9mm.obj", 1.5f));
        shape->setMargin(0.f);

        texture2d.load("bullet", true);
        bulletShaderProgram.load("shaders/bullet_vert.glsl", "shaders/bullet_frag.glsl");
    }

    Bullet(World &world, const CollisionObject &creator, btTransform startTransform, glm::vec3 impulse)
      : PhysicsObject(world, shape.get(), mass, {0, 0, 0}), 
        creatorCollisionObjectId(creator.getId()),
        world(world)
    {
        setTransform(startTransform);
        applyCentralImpulse(impulse);
    } 

    virtual ~Bullet()
    {
        explode();
    }

    bool isAlive()
    {
        return alive;
    }

    static constexpr float mass = 1.f;

protected:
    void contactAddedCallback(CollisionObject *other) override
    {
        if (other->getId() != creatorCollisionObjectId)
            kill();
    }

    const Texture2d &getTexture0() const override { return texture2d; }
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return bulletShaderProgram; }

    void explode();

private:
    CollisionObject::idType creatorCollisionObjectId;
    World& world;

    static inline ShaderProgram bulletShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture2d;
    static inline std::unique_ptr<btCollisionShape> shape;
    static inline std::function<void(const glm::vec3 &)> explosion;

    static inline constexpr float explosionRadius = 5.f;
    static inline constexpr float fullDamageRadius = 2.f;
    static inline constexpr float damageBase = 50.f;
    inline static btSphereShape explosionShape = btSphereShape(explosionRadius);
    inline static GhostObject ghostObject = GhostObject(nullptr, &explosionShape);
};

#endif
