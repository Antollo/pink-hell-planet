#ifndef WINDOW_H_
#define WINDOW_H_

#include <queue>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ShaderProgram.h"

class Window;

class Drawable
{
public:
    virtual void draw(Window *window) const = 0;
};

//Current implementation can use only one window

class Window
{
public:
    Window(int width, int height, const char *title) : _width(width), _height(height), xDiff(0.0), yDiff(0.0), xPosOld(0.0), yPosOld(0.0), firstUpdate(true)
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

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);

        glfwMakeContextCurrent(window);

        glfwSwapInterval(0);

        if (!gladLoadGL())
            errorAndExit("gladLoadGL failed");

        P = glm::perspective(glm::radians(50.f), float(_width / _height), 0.01f, 1000.0f);
        ShaderProgram::setMatrixP(P);

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
        if (firstUpdate)
        {
            ShaderProgram::setMatrixP(P);
            firstUpdate = false;
        }
        xDiff = 0.0;
        yDiff = 0.0;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    int pollKey()
    {
        if(!keys.empty())
        {
            int key = keys.front();
            keys.pop();
            return key; 
        }
        return 0;
    }

    void draw(const Drawable &drawable)
    {
        drawable.draw(this);
    }

    void getCursorDiff(float &x, float &y)
    {
        x = xDiff;
        y = yDiff;
    }

    void refreshMatrixP()
    {
        P = glm::perspective(glm::radians(50.f), float(_width / _height), 0.01f, 1000.0f);
        ShaderProgram::setMatrixP(P);
    }

private:
    friend class Drawable;

    glm::mat4 P;
    GLFWwindow *window;

    bool firstUpdate;
    double _width, _height;
    std::queue<int> keys;
    double xPosOld, yPosOld;
    float xDiff, yDiff;
    static void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        obj._width = width;
        obj._height = height;
        obj.P = glm::perspective(glm::radians(50.f), float(obj._width / obj._height), 0.01f, 1000.0f);
        ShaderProgram::setMatrixP(obj.P);
    }
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        switch (action)
        {
        case GLFW_PRESS:
            obj.keys.push(key);
            break;
        
        case GLFW_RELEASE:
            obj.keys.push(-key);
            break;
        }
    }
    static void cursorPosCallback(GLFWwindow *window, double xPos, double yPos)
    {
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        if (obj.xPosOld == 0.0 && obj.yPosOld == 0.0 && obj.xDiff == 0 && obj.yDiff == 0)
        {
            obj.xPosOld = xPos;
            obj.yPosOld = yPos;
        }
        else
        {
            obj.xDiff = xPos - obj.xPosOld;
            obj.yDiff = yPos - obj.yPosOld;
            obj.xPosOld = xPos;
            obj.yPosOld = yPos;
        }
    }
};

#endif /* !RENDERER_H_ */