#ifndef BULLET_H_
#define BULLET_H_

#include "PhysicsObject.h"

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

    Bullet(World &world) : PhysicsObject(world, shape.get(), mass, {40, 8, 10}), alive(true) {}
    ~Bullet()
    {
        explosion(getPosition());
    }

    static void setExplosionCallback(std::function<void(const glm::vec3 &)> e)
    {
        explosion = e;
    }

    bool isAlive()
    {
        return alive;
    }

    static constexpr float mass = 1.f;

protected:
    const Texture2d &getTexture0() const override { return texture2d; }
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return bulletShaderProgram; }
    void contactAddedCallback(CollisionObject *other) override
    {
        alive = false;
    }

private:
    bool alive;
    static inline ShaderProgram bulletShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture2d;
    static inline std::unique_ptr<btCollisionShape> shape;
    static inline std::function<void(const glm::vec3 &)> explosion;
};

#endif
