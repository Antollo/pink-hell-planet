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
        defaultTexture3d.load("skybox", true);
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
        if (getTexture3d().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, getTexture3d().getTextureId());
            getShaderProgram().setUniform1i("cube", 4);
        }
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(GL_TRIANGLES, 0, getVertexArray().getLength());
        glBindVertexArray(0);
    }

protected:
    glm::mat4 M;

    virtual const VertexArray &getVertexArray() const = 0;
    virtual const ShaderProgram &getShaderProgram() const = 0;
    virtual const Texture2d &getTexture0() const { return defaultTexture; }
    virtual const Texture2d &getTexture1() const { return defaultTexture; }
    virtual const Texture2d &getTexture2() const { return defaultTexture; }
    virtual const Texture2d &getTexture3() const { return defaultTexture; }
    virtual const Texture3d &getTexture3d() const { return defaultTexture3d; }

    static inline Texture2d defaultTexture;
    static inline Texture3d defaultTexture3d;
    static inline VertexArray defaultVertexArray;
    static inline ShaderProgram defaultShaderProgram;
};

#endif /* !DRAWABLEOBJECT_H_ */
