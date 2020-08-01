#ifndef FIREFLIES_H
#define FIREFLIES_H

#include "DrawableObject.h"

class Fireflies : public DrawableObject
{
public:
    static void init()
    {
        firefliesShaderProgram.load("shaders/fireflies_vert.glsl", "shaders/fireflies_geom.glsl", "shaders/fireflies_frag.glsl");
        vertexArray.load({0.f, 0.f, 0.f});
        texture.load("firefly_DIFF", true, true);
    }
    void draw(Window *window) const override
    {
        getShaderProgram().use();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
        getShaderProgram().setUniform1i("tex0", 0);
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(GL_POINTS, 0, getVertexArray().getLength());
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return firefliesShaderProgram; }
    const Texture2d &getTexture0() const override { return texture; }

private:
    static inline ShaderProgram firefliesShaderProgram;
    static inline VertexArray vertexArray;
    static inline Texture2d texture;
};

#endif /* FIREFLIES_H */
