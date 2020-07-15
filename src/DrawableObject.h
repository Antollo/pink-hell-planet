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
        defaultTextureShaderProgram.load("shaders/tex_vert.glsl", "shaders/tex_geom.glsl", "shaders/tex_frag.glsl");
    }
    DrawableObject() : M(1.0f) {}
    virtual void update(float delta) {}
    void draw(Window *window) const override
    {
        getShaderProgram().use();
        getShaderProgram().setUniformMatrix4fv("M", M);
        if (getTexture0().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
            getShaderProgram().setUniform1i("tex0", 0);
        }
        if (getTexture1().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, getTexture1().getTextureId());
            getShaderProgram().setUniform1i("tex1", 1);
        }
        if (getTexture2().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, getTexture2().getTextureId());
            getShaderProgram().setUniform1i("tex2", 2);
        }
        if (getTexture3().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, getTexture3().getTextureId());
            getShaderProgram().setUniform1i("tex3", 3);
        }
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(GL_TRIANGLES, 0, getVertexArray().getLength());
        glBindVertexArray(0);
    }

protected:
    glm::mat4 M;

    virtual const VertexArray &getVertexArray() const = 0;
    virtual const ShaderProgram &getShaderProgram() const { return defaultColorShaderProgram; }
    virtual const Texture &getTexture0() const { return defaultTexture; }
    virtual const Texture &getTexture1() const { return defaultTexture; }
    virtual const Texture &getTexture2() const { return defaultTexture; }
    virtual const Texture &getTexture3() const { return defaultTexture; }

    static inline Texture defaultTexture;
    static inline ShaderProgram defaultColorShaderProgram, defaultTextureShaderProgram;
};

#endif /* !DRAWABLEOBJECT_H_ */
