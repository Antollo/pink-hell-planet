#ifndef FIREFLIES_H
#define FIREFLIES_H

#include "DrawableObject.h"

class Fireflies : public DrawableObject
{
public:
    static void init()
    {
        firefliesShaderProgram.load("shaders/fireflies_vert.glsl", "shaders/fireflies_geom.glsl", "shaders/fireflies_frag.glsl");
        vertexArray.loadVertices({0.f, 0.f, 0.f});
        texture.load("firefly_DIFF", true, true);
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return firefliesShaderProgram; }
    const Texture2d &getTexture0() const override { return texture; }
    bool getBlend() const override { return true; };
    GLenum getMode() const override { return GL_POINTS; };
    bool getDepthMask() const override { return false; };

private:
    static inline ShaderProgram firefliesShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture;
};

#endif /* FIREFLIES_H */
