#ifndef DUMMYMODEL_H_
#define DUMMYMODEL_H_

#include "PlayableObject.h"
#include "Axes.h"
#include "Bazooka.h"

class DummyModel : public PlayableObject
{
public:
    static void init()
    {
        shape = PhysicsObject::boundingBoxShape(vertexArray.load("TV.obj", 70.f));
        shape->setMargin(0.f);
        
        texture0.load("TV_DIFF", true);
        texture1.load("TV_SPEC", true);
        texture2.load("TV_NRM");
        texture3.load("TV_GLS");

        tvShaderProgram.load("shaders/tv_vert.glsl", "shaders/tv_frag.glsl");
    }
    DummyModel(World &world) : PlayableObject(world, shape.get(), std::make_unique<Bazooka>()) {}
    virtual ~DummyModel() {}

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return tvShaderProgram; }
    const Texture2d &getTexture0() const override { return texture0; }
    const Texture2d &getTexture1() const override { return texture1; }
    const Texture2d &getTexture2() const override { return texture2; }
    const Texture2d &getTexture3() const override { return texture3; }

private:
    static inline ShaderProgram tvShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture0, texture1, texture2, texture3;
    static inline std::unique_ptr<btCollisionShape> shape;
};

#endif /* !DUMMYMODEL_H_ */
