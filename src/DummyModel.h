#ifndef DUMMYMODEL_H_
#define DUMMYMODEL_H_

#include "PhysicsObject.h"
#include "Axes.h"

class DummyModel : public PhysicsObject
{
public:
    static void init()
    {
        shape = shapeFromVertices(vertexArray.load("PenguinBaseMesh.obj"));
        texture.load("Penguin Diffuse Color.png");
    }
    static void fini()
    {
        shape.reset();
    }
    DummyModel(World &world) : PhysicsObject(world, shape.get()) {}
    void update(double delta) override
    {
        PhysicsObject::update(delta);
        axes.setMatrixM(M);
    }
    void draw(Window *window) const override
    {
        DrawableObject::draw(window);
        window->draw(axes);
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return defaultTextureShaderProgram; }
    const Texture &getTexture() const override { return texture; }

private:
    Axes axes;
    static inline VertexArray vertexArray;
    static inline Texture texture;
    static inline std::unique_ptr<btCollisionShape> shape;
};

#endif /* !DUMMYMODEL_H_ */
