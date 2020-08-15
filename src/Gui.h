#ifndef GUI_H_
#define GUI_H_

#include "DrawableObject.h"

class GuiObject : public DrawableObject
{
public:
    GuiObject() : color(1.f, 1.f, 1.f, 1.f), position(0.f, 0.f)
    {
    }
    virtual void setPosition(const glm::vec2 &newPosition)
    {
        position = newPosition;
        M = glm::translate(glm::vec3(newPosition.x, newPosition.y, 0.f));
    }
    virtual void setColor(const glm::vec4 &newColor)
    {
        color = newColor;
    }

protected:
    float getAlpha() const override
    {
        return color.a;
    }
    Window::ProjectionMode getProjectionMode() const override
    {
        return Window::ProjectionMode::ortho;
    };
    glm::vec4 color;
    glm::vec2 position;
};

#endif /* GUI_H_ */
