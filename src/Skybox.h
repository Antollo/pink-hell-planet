#ifndef SKYBOX_H
#define SKYBOX_H

#include "DrawableObject.h"

class Skybox : public DrawableObject
{
public:
    static void init()
    {
        skyboxShaderProgram.load("shaders/sky_vert.glsl", "shaders/sky_frag.glsl");
        vertexArray.loadVertices({
                          -1.0f, 1.0f, -1.0f,
                          -1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f,
                          1.0f, 1.0f, -1.0f,
                          -1.0f, 1.0f, -1.0f,

                          -1.0f, -1.0f, 1.0f,
                          -1.0f, -1.0f, -1.0f,
                          -1.0f, 1.0f, -1.0f,
                          -1.0f, 1.0f, -1.0f,
                          -1.0f, 1.0f, 1.0f,
                          -1.0f, -1.0f, 1.0f,

                          1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f,

                          -1.0f, -1.0f, 1.0f,
                          -1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, -1.0f, 1.0f,
                          -1.0f, -1.0f, 1.0f,

                          -1.0f, 1.0f, -1.0f,
                          1.0f, 1.0f, -1.0f,
                          1.0f, 1.0f, 1.0f,
                          1.0f, 1.0f, 1.0f,
                          -1.0f, 1.0f, 1.0f,
                          -1.0f, 1.0f, -1.0f,

                          -1.0f, -1.0f, -1.0f,
                          -1.0f, -1.0f, 1.0f,
                          1.0f, -1.0f, -1.0f,
                          1.0f, -1.0f, -1.0f,
                          -1.0f, -1.0f, 1.0f,
                          1.0f, -1.0f, 1.0f});
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return skyboxShaderProgram; }
private:
    static inline ShaderProgram skyboxShaderProgram;
    static inline VertexArray vertexArray;
};

#endif /* SKYBOX_H */
