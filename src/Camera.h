
#ifndef CAMERA_H_
#define CAMERA_H_

#include "Window.h"
#include "PlayableObject.h"

#include "debug.h"

class Camera
{
public:
    Camera(Window &w, PlayableObject *&p) : window(w), player(p)
    {
        glm::mat4 V = glm::lookAt(
            glm::vec3(0.f, 0.f, 0.f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        ShaderProgram::setMatrixV(V);
    }

    void consumeKey(int key)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            forward = true;
            break;
        case -GLFW_KEY_W:
            forward = false;
            break;

        case GLFW_KEY_S:
            backward = true;
            break;
        case -GLFW_KEY_S:
            backward = false;
            break;

        case GLFW_KEY_A:
            left = true;
            break;
        case -GLFW_KEY_A:
            left = false;
            break;

        case GLFW_KEY_D:
            right = true;
            break;
        case -GLFW_KEY_D:
            right = false;
            break;
        }
    }

    void update(float delta)
    {
        float distance = freecamSpeed * delta;

        if (forward)
            position += frontDirection * distance;
        if (backward)
            position -= frontDirection * distance;

        if (right)
            position -= rightDirection * distance;
        if (left)
            position += rightDirection * distance;

        window.getCursorDiff(xCursorDiff, yCursorDiff);

        yaw += yCursorDiff * mouseSensitivity;
        pitch -= xCursorDiff * mouseSensitivity;

        frontDirection.z = sin(yaw) * cos(pitch);
        frontDirection.x = sin(yaw) * sin(pitch);
        frontDirection.y = cos(yaw);

        rightDirection.x = cos(pitch);
        rightDirection.y = 0.f;
        rightDirection.z = -sin(pitch);

        upDirection = glm::cross(frontDirection, rightDirection);

        if (player != nullptr)
        {
            position = (player->getPosition() - glm::normalize(frontDirection)*1.5f + glm::vec3(0.f, 1.f, 0.f)) * 0.2f + oldPosition * 0.8f;
            oldPosition = position;
        }

        V = glm::lookAt(
            position,
            position + glm::normalize(frontDirection),
            glm::normalize(upDirection));

        ShaderProgram::setMatrixV(V);
    }

private:
    Window& window;
    PlayableObject*& player;
    glm::mat4 V;
    glm::vec3 position, frontDirection, upDirection, rightDirection, oldPosition = glm::vec3(0.f, 0.f, 0.f);
    float xCursorDiff, yCursorDiff, yaw = 0, pitch = 0;
    bool forward = false, backward = false, left = false, right = false;
    static constexpr float freecamSpeed = 1.f;
    static constexpr float mouseSensitivity = 0.004f;
};

#endif /* !CAMERA_H_ */