#ifndef GUI_H_
#define GUI_H_

#include "DrawableObject.h"

class GuiObject : public DrawableObject
{
public:
    static void init()
    {
        guiShaderProgram.load("shaders/gui_vert.glsl", "shaders/gui_frag.glsl");
    }

    GuiObject()
        : color(1.f, 1.f, 1.f, 1.f), position(0.f, 0.f), positionOffset(0.f, 0.f)
    {
        setPosition({0.f, 0.f});
        setAlpha(color.a);
    }
    virtual ~GuiObject() {}

    void setPosition(const glm::vec2 &newPosition)
    {
        position = newPosition;
        updateMatrixM();
    }
    void setPositionOffset(const glm::vec2 &newPositionOffset)
    {
        positionOffset = newPositionOffset;
        updateMatrixM();
    }
    virtual void setColor(const glm::vec4 &newColor)
    {
        color = newColor;
        setAlpha(color.a);
    }
    void draw(Window *window) const override
    {
        ShaderProgram::setColor(color);
        DrawableObject::draw(window);
    }
    static void setAspectRatioGlobalScale(float newAspectRatio, float newScale)
    {
        aspectRatio = newAspectRatio;
        globalScale = newScale;
    }
    void update(float delta) override
    {
        DrawableObject::update(delta);
        setAlpha(color.a);
        updateMatrixM();
    }
    float getScale() const { return scale; }
    void setScale(float newScale) { scale = newScale; }

protected:
    Window::ProjectionMode getProjectionMode() const override
    {
        return Window::ProjectionMode::ortho;
    };
    const ShaderProgram &getShaderProgram() const override
    {
        return guiShaderProgram;
    }
    static float getAspectRatio()
    {
        return aspectRatio;
    }
    static float getGlobalScale()
    {
        return globalScale;
    }
    glm::vec4 color;
    glm::vec2 position;
    glm::vec2 positionOffset;

private:
    void updateMatrixM()
    {
        M = glm::scale(
            glm::translate(glm::vec3(position.x + positionOffset.x * getGlobalScale(), position.y + positionOffset.y * getGlobalScale() * getAspectRatio(), 0.f)),
            scale * glm::vec3(getGlobalScale(), getGlobalScale() * getAspectRatio(), getGlobalScale()));
    }
    float scale = 1.f;
    static inline ShaderProgram guiShaderProgram;
    static inline float aspectRatio = 1.f;
    static inline float globalScale = 0.001f;
};

#endif /* GUI_H_ */
