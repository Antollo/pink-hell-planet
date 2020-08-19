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

    Bullet(World &world) : PhysicsObject(world, shape.get(), 0.f, {40, 8, 10}) {}

protected:
    const Texture2d &getTexture0() const override { return texture2d; }
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return bulletShaderProgram; }

private:
    static inline ShaderProgram bulletShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture2d;
    static inline std::unique_ptr<btCollisionShape> shape;
};

#endif
