#include <iostream>
#include <cmath>
#include "Window.h"
#include "DrawableObject.h"
#include "loader.h"

int main()
{
    Window window(800, 600, "game");
    DrawableObject::init();
    DummyModel::init();
    DummyModel obj;

    glm::mat4 V = glm::lookAt(
        glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    ShaderProgram::setGlobalUniformMatrix4fv("V", V);

    glm::vec3 position(0.f, 0.f, 0.f), frontDirection, upDirection, rightDirection;
    float xCursorPos, yCursorPos, yaw = 0.f, pitch = 90.f, time, lastTime = 0, timeDiff;

    int frames = 0;

    while (window.isOpen())
    {
        frames++;
        time = glfwGetTime();
        timeDiff = time - lastTime;
        if (time >= 2.f)
        {
            glfwSetTime(0.f);
            lastTime = 0;
            std::cerr << "fps: " << frames / 2.f << std::endl;
            frames = 0;
        }
        else
            lastTime = time;

        static const float freecamSpeed = 100;

        float distance = freecamSpeed * timeDiff;

        if (window.getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS)
            window.close();

        if (window.getKeyState(GLFW_KEY_W) == GLFW_PRESS)
            position += frontDirection * distance;
        if (window.getKeyState(GLFW_KEY_S) == GLFW_PRESS)
            position -= frontDirection * distance;

        if (window.getKeyState(GLFW_KEY_D) == GLFW_PRESS)
            position -= rightDirection * distance;
        if (window.getKeyState(GLFW_KEY_A) == GLFW_PRESS)
            position += rightDirection * distance;

        window.getCursorPosition(xCursorPos, yCursorPos);
        window.setCursorPositionCenter();

        static const float mouseSensitivity = 0.005f;

        yaw += yCursorPos * mouseSensitivity;
        pitch -= xCursorPos * mouseSensitivity;

        if (pitch > 160.0f)
            pitch = 160.0f;
        if (pitch < 20.0f)
            pitch = 20.0f;

        frontDirection.z = sin(yaw) * cos(pitch);
        frontDirection.x = sin(yaw) * sin(pitch);
        frontDirection.y = cos(yaw);

        rightDirection.x = cos(pitch);
        rightDirection.y = 0.0;
        rightDirection.z = -sin(pitch);

        upDirection = glm::cross(frontDirection, rightDirection);

        V = glm::lookAt(
            position,
            position + glm::normalize(frontDirection),
            glm::normalize(upDirection));
        ShaderProgram::setGlobalUniformMatrix4fv("V", V);

        if (time < 1.f)
            window.setClearColor(0.5f * time, time, 1.f - time);
        else
            window.setClearColor(0.5f - 0.5f * time, 2.f - time, time - 1.f);
        window.clear();
        window.draw(obj);

        window.swapBuffers();
    }
    return 0;
}
