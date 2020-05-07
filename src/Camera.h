
#ifndef CAMERA_H_
#define CAMERA_H_

#include "Window.h"
#include "PlayableObject.h"

class Camera
{
public:
    Camera(Window &w, PlayableObject *&p) : window(w), player(p), forward(false), backward(false), left(false), right(false), oldPosition(0.f, 0.f, 0.f)
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

        /*if (yaw > 160.0f)
            yaw = 160.0f;
        if (yaw < 20.0f)
            yaw = 20.0f;
        */

        frontDirection.z = sin(yaw) * cos(pitch);
        frontDirection.x = sin(yaw) * sin(pitch);
        frontDirection.y = cos(yaw);

        rightDirection.x = cos(pitch);
        rightDirection.y = 0.f;
        rightDirection.z = -sin(pitch);

        upDirection = glm::cross(frontDirection, rightDirection);

        if (player != nullptr)
        {
            //position = glm::vec3(0.f, 0.f, 0.f);
            //glm::mat3 rotMat(player->getM());
            //glm::vec3 d(player->getM()[3]);
            //position = position* 0.6f + d * rotMat * 0.4f;
            //oldPosition = position;
            //position = player->getM()*oldPosition;
            position = (player->getPosition() - glm::normalize(frontDirection)*1.5f + glm::vec3(0.f, 1.f, 0.f)) * 0.2f + position * 0.8f;
            oldPosition = position;
        }

        V = glm::lookAt(
            position,
            position + glm::normalize(frontDirection),
            glm::normalize(upDirection));

        //if (player == nullptr)
        ShaderProgram::setMatrixV(V);
        //else
        //    ShaderProgram::setMatrixV(V*glm::inverse(player->getM()));
    }

private:
    Window& window;
    PlayableObject*& player;
    glm::mat4 V;
    glm::vec3 position, frontDirection, upDirection, rightDirection, oldPosition;
    float xCursorDiff, yCursorDiff, yaw, pitch;
    bool forward, backward, left, right;
    static constexpr float freecamSpeed = 1.f;
    static constexpr float mouseSensitivity = 0.004f;
};

#endif /* !CAMERA_H_ */