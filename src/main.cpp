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
    float xCursorPos, yCursorPos, yaw = 0.f, pitch = 90.f, time;

    while (window.isOpen())
    {
        time = glfwGetTime();
        if (time >= 2.f)
            glfwSetTime(0.f);

        if (window.getKeyState(GLFW_KEY_ESCAPE) == GLFW_PRESS)
            window.close();

        if (window.getKeyState(GLFW_KEY_W) == GLFW_PRESS)
            position += frontDirection;
        if (window.getKeyState(GLFW_KEY_S) == GLFW_PRESS)
            position -= frontDirection;

        if (window.getKeyState(GLFW_KEY_D) == GLFW_PRESS)
            position -= rightDirection;
        if (window.getKeyState(GLFW_KEY_A) == GLFW_PRESS)
            position += rightDirection;

        window.getCursorPosition(xCursorPos, yCursorPos);
        window.setCursorPositionCenter();

        yaw += yCursorPos * 0.005f;
        pitch -= xCursorPos * 0.005f;

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
            window.setClearColor(1.f - time, time, 0.5f * time);
        else
            window.setClearColor(time - 1.f, 2.f - time, 0.5f - 0.5f * time);
        window.clear();
        window.draw(obj);

        window.swapBuffers();
    }
    return 0;
}