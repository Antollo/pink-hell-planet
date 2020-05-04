#ifndef DRAWABLEOBJECT_H_
#define DRAWABLEOBJECT_H_

#include <cassert>
#include <glad/glad.h>
#include "Window.h"
#include "Texture.h"
#include "VertexArray.h"

class DrawableObject : public Drawable
{
public:
    static void init()
    {
        defaultColorShaderProgram.load("shaders/color_vert.glsl", "shaders/color_frag.glsl");
        defaultTextureShaderProgram.load("shaders/tex_vert.glsl", "shaders/tex_frag.glsl");
    }
    DrawableObject() : M(1.0f) {}
    virtual void update(double delta) {}
    void draw(Window *window) const override
    {
        getShaderProgram().use();
        getShaderProgram().setUniformMatrix4fv("M", M);
        if (getTexture().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, getTexture().getTextureId());
            getShaderProgram().setUniform1i("tex", 0);
        }
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(GL_TRIANGLES, 0, getVertexArray().getLength());
        glBindVertexArray(0);
    }

protected:
    glm::mat4 M;

    virtual const VertexArray &getVertexArray() const = 0;
    virtual const ShaderProgram &getShaderProgram() const { return defaultColorShaderProgram; }
    virtual const Texture &getTexture() const { return defaultTexture; }

    static inline Texture defaultTexture;
    static inline ShaderProgram defaultColorShaderProgram, defaultTextureShaderProgram;
};

#endif /* !DRAWABLEOBJECT_H_ */
