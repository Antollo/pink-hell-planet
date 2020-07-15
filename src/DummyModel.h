#ifndef DUMMYMODEL_H_
#define DUMMYMODEL_H_

#include "PlayableObject.h"
#include "Axes.h"

class DummyModel : public PlayableObject
{
public:
    static void init()
    {
        shape = shapeFromVertices(vertexArray.load("PenguinBaseMesh.obj"));
        texture.load("Penguin Diffuse Color.png");
    }
    DummyModel(World &world) : PlayableObject(world, shape.get()) {}
    void update(float delta) override
    {
        PlayableObject::update(delta);
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
    const Texture &getTexture0() const override { return texture; }

private:
    Axes axes;
    static inline VertexArray vertexArray;
    static inline Texture texture;
    static inline std::unique_ptr<btCollisionShape> shape;
};

#endif /* !DUMMYMODEL_H_ */
