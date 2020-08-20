
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
        update(0);
    }

    void consumeKey(int glfwKeyCode)
    {
        switch (glfwKeyCode)
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

        case GLFW_KEY_KP_ADD:
            viewDistance += 0.1f;
            break;
        case GLFW_KEY_KP_SUBTRACT:
            viewDistance -= 0.1f;
            break;
        }
    }
    void consumeCursorDiff(float xCursorDiff, float yCursorDiff)
    {
        yaw += yCursorDiff * mouseSensitivity;
        pitch -= xCursorDiff * mouseSensitivity;
    }

    void update(float delta)
    {
        if (player == nullptr)
        {
            freeCamPositionUpdate(delta);
            frontDirection.z = sin(yaw) * cos(pitch);
            frontDirection.x = sin(yaw) * sin(pitch);
            frontDirection.y = cos(yaw);
            frontDirection = glm::normalize(frontDirection);
        }
        else
        {
            frontDirection = player->getFrontDirection();
        }

        rightDirection.x = cos(pitch);
        rightDirection.y = 0.f;
        rightDirection.z = -sin(pitch);

        upDirection = glm::cross(frontDirection, rightDirection);

        if (player != nullptr)
        {
            float newPosWeight = std::min(newPositionFactor * delta, 1.f);
            position = (player->getPosition() - frontDirection * viewDistance + glm::vec3(0.f, 0.5f, 0.f)) * newPosWeight + oldPosition * (1 - newPosWeight);
            oldPosition = position;
            averagePosition = player->getPosition() * player->getZoom() + position * (1 - player->getZoom());
        }
        else
        {
            averagePosition = position;
        }

        V = glm::lookAt(
            averagePosition,
            averagePosition + frontDirection,
            glm::normalize(upDirection));

        ShaderProgram::setMatrixV(V);
    }

    const PlayableObject *getPlayer() const { return player; }
    glm::vec3 getFrontDirection() const { return frontDirection; }

private:
    void updateYawPitch()
    {
        if (player != nullptr)
        {
            yaw = player->getYaw();
            pitch = player->getPitch();
        }
    }
    void freeCamPositionUpdate(float delta)
    {
        float distance = freecamSpeed * delta;
        if (player == nullptr)
        {
            if (forward)
                position += frontDirection * distance;
            if (backward)
                position -= frontDirection * distance;

            if (right)
                position -= rightDirection * distance;
            if (left)
                position += rightDirection * distance;
        }
    }

    Window &window;
    PlayableObject *&player;
    glm::mat4 V;
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), frontDirection, upDirection, rightDirection, averagePosition, oldPosition = glm::vec3(0.f, 0.f, 0.f);
    float xCursorDiff, yCursorDiff, yaw = 0, pitch = 0, viewDistance = 6.f;
    bool forward = false, backward = false, left = false, right = false;
    static constexpr float freecamSpeed = 10.f;
    static constexpr float mouseSensitivity = 0.004f;
    static constexpr float newPositionFactor = 26.f;
};

#endif /* !CAMERA_H_ */
