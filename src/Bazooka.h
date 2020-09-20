#ifndef C848B005_14B7_4990_9106_FB317D41EE9F
#define C848B005_14B7_4990_9106_FB317D41EE9F

#include "DrawableObject.h"

class Bazooka : public ManagedDrawableObject
{
public:
    static void init()
    {
        vertexArray.load("bazooka.obj", 20.f, {0});

        texture0.load("bazooka_DIFF", true);
        texture1.load("bazooka_SPEC", true);
        texture2.load("bazooka_NRM");
        texture3.load("bazooka_GLS");

        bazookaShaderProgram.load("shaders/tv_vert.glsl", "shaders/tv_frag.glsl");
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return bazookaShaderProgram; }
    const Texture2d &getTexture0() const override { return texture0; }
    const Texture2d &getTexture1() const override { return texture1; }
    const Texture2d &getTexture2() const override { return texture2; }
    const Texture2d &getTexture3() const override { return texture3; }

private:
    static inline ShaderProgram bazookaShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture0, texture1, texture2, texture3;
};

#endif /* C848B005_14B7_4990_9106_FB317D41EE9F */
