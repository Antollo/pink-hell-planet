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
        //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
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
            std::cout << width << " " << height << std::endl;
            glViewport(0, 0, width, height);
            Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            obj._width = width;
            obj._height = height;
            obj.P = glm::perspective(glm::radians(50.f), float(obj._width / obj._height), 0.01f, 1000.0f);
            ShaderProgram::setGlobalUniformMatrix4fv("P", obj.P);
        });

        glfwMakeContextCurrent(window);

        if (!gladLoadGL())
            errorAndExit("gladLoadGL failed");

        P = glm::perspective(glm::radians(50.f), float(_width / _height), 0.1f, 1000.0f);
        ShaderProgram::setGlobalUniformMatrix4fv("P", P);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
        x = wx - _width / 2;
        y = wy - _height / 2;
    }

    void setCursorPositionCenter()
    {
        glfwSetCursorPos(window, _width / 2.0, _height / 2.0);
        //float wx, wy;
        //getCursorPosition(wx, wy);
        //std::cout << wx << " " << wy << std::endl;
    }

private:
    friend class Drawable;
    glm::mat4 P;
    GLFWwindow *window;
    double _width, _height;
};

#endif /* !RENDERER_H_ */
