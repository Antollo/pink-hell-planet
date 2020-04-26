#ifndef WINDOW_H_
#define WINDOW_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Window;

class Drawable
{
public:
    virtual void draw(Window *window) const = 0;
};

class Window
{
public:
    Window(int width, int height, const char *title) : _width(width), _height(height)
    {
        if (!glfwInit())
            errorAndExit("glfwInit failed");

        glfwSetErrorCallback([](int code, const char *description) {
            error("Error: ", description);
        });

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_SAMPLES, 4);
        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
            errorAndExit("glfwCreateWindow failed");

        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
            glViewport(0, 0, width, height);
            Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            obj.P = glm::perspective(glm::radians(50.f), (float)width / (float)height, 0.01f, 1000.0f);
            ShaderProgram::setGlobalUniformMatrix4fv("P", obj.P);
            obj._width = width;
            obj._height = height;
        });

        glfwMakeContextCurrent(window);

        if (!gladLoadGL())
            errorAndExit("gladLoadGL failed");

        P = glm::perspective(glm::radians(50.f), (float)width / (float)height, 0.1f, 1000.0f);
        ShaderProgram::setGlobalUniformMatrix4fv("P", P);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
    }

    ~Window()
    {
        glfwTerminate();
    }

    bool isOpen() const
    {
        return !glfwWindowShouldClose(window);
    }

    void setClearColor(float r, float g, float b)
    {
        glClearColor(r, g, b, 1.0f);
    }

    void clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void close()
    {
        glfwSetWindowShouldClose(window, true);
    }

    void swapBuffers()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    int getKeyState(int key)
    {
        return glfwGetKey(window, key);
    }

    void draw(const Drawable &drawable)
    {
        drawable.draw(this);
    }

    void getCursorPosition(float &x, float &y)
    {
        double wx, wy;
        glfwGetCursorPos(window, &wx, &wy);
        x = float(wx) - float(_width) / 2.f;
        y = float(wy) - float(_height) / 2.f;
    }

    void setCursorPositionCenter()
    {
        glfwSetCursorPos(window, double(_width) / 2.0, double(_height) / 2.0);
    }

private:
    friend class Drawable;
    glm::mat4 P;
    GLFWwindow *window;
    int _width, _height;
};

#endif /* !RENDERER_H_ */
