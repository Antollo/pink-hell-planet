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
    enum class MouseButton
    {
        none = 0,
        left = 1,
        right = 2
    };

    struct MouseButtonEvent
    {
        MouseButtonEvent() : button(MouseButton::none), down(false) {}
        MouseButtonEvent(MouseButton b, bool d) : button(b), down(d) {}
        operator bool() const { return button != MouseButton::none; }
        MouseButton button;
        bool down;
    };

    struct MouseScrollEvent
    {
        MouseScrollEvent() : x(0.f), y(0.f) {}
        MouseScrollEvent(float xOffset, float yOffset) : x(xOffset), y(yOffset) {}
        MouseScrollEvent(double xOffset, double yOffset) : x(xOffset), y(yOffset) {}
        operator bool() const { return x != 0.f || y != 0.f; }
        float x, y;
    };

    Window(int width, int height, const char *title, bool visible = false)
        : _width(width), _height(height),
          xPosOld(0.0), yPosOld(0.0),
          xDiff(0.0), yDiff(0.0),
          firstUpdate(true), projectionMode(ProjectionMode::perspective),
          orthoP(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f))
    {
        if (!glfwInit())
            errorAndExit("glfwInit failed");

        glfwSetErrorCallback([](int code, const char *description) {
            error("Error: ", description);
        });

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        //glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        glfwWindowHint(GLFW_SAMPLES, 8);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);

        window = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if (!window)
            errorAndExit("glfwCreateWindow failed");

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetScrollCallback(window, mouseScrollCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);

        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);

        if (!gladLoadGL())
            errorAndExit("gladLoadGL failed");

        std::cout << "OpenGL context version: " << reinterpret_cast<const char *>(glGetString(GL_VERSION)) << std::endl;

        perspectiveP = glm::perspective(glm::radians(50.f), getAspectRatio(), 0.01f, 1000.0f);

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    ~Window()
    {
        glfwDestroyWindow(window);

        // glfwTerminate has to be called after destructors of all Textures and ShaderPrograms
        // and there are some static Textures.
        // As a temporary solution just dont call glfwTerminate at all.
        // Seems to work properly at least on linux, OS frees all resources anyway.
        // glfwTerminate();
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

    void focus()
    {
        glfwFocusWindow(window);
    }

    void swapBuffers()
    {
        if (firstUpdate)
        {
            ShaderProgram::setMatrixP(perspectiveP);
            firstUpdate = false;
        }
        xDiff = 0.0;
        yDiff = 0.0;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    int pollKeyEvent() { return popOrZero(keyQueue); }

    MouseButtonEvent pollMouseButtonEvent() { return popOrZero(mouseButtonEventQueue); }

    MouseScrollEvent pollMouseScrollEvent() { return popOrZero(mouseScrollEventQueue); }

    void draw(const Drawable &drawable)
    {
        drawable.draw(this);
    }

    void getCursorDiff(float &x, float &y)
    {
        x = xDiff;
        y = yDiff;
    }

    float getAspectRatio() const { return _width / _height; }
    float getHeight() const { return _height; }
    float getWidth() const { return _width; }

    enum class ProjectionMode
    {
        perspective,
        ortho
    };

    void setProjectionMode(ProjectionMode newProjectionMode)
    {
        if (projectionMode != newProjectionMode)
        {
            if (newProjectionMode == ProjectionMode::ortho)
                ShaderProgram::setMatrixP(orthoP);
            else if (newProjectionMode == ProjectionMode::perspective)
                ShaderProgram::setMatrixP(perspectiveP);
            projectionMode = newProjectionMode;
        }
    }

    void hide()
    {
        glfwHideWindow(window);
    }

    void show()
    {
        glfwShowWindow(window);
    }

    void toggleFullscreen()
    {
        if (glfwGetWindowMonitor(window) == nullptr)
        {
            auto monitor = glfwGetPrimaryMonitor();
            if (monitor)
            {
                glfwGetWindowPos(window, &oldPosition.x, &oldPosition.y);
                glfwGetWindowSize(window, &oldSize.x, &oldSize.y);
                auto mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            }
        }
        else
        {
            glfwSetWindowMonitor(window, nullptr, oldPosition.x, oldPosition.y, oldSize.x, oldSize.y, 0);
        }
    }

private:
    friend class Drawable;

    GLFWwindow *window;
    float _width, _height;
    glm::ivec2 oldSize, oldPosition;
    std::queue<int> keyQueue;
    std::queue<MouseButtonEvent> mouseButtonEventQueue;
    std::queue<MouseScrollEvent> mouseScrollEventQueue;
    double xPosOld, yPosOld;
    float xDiff, yDiff;
    bool firstUpdate;
    ProjectionMode projectionMode;
    glm::mat4 perspectiveP;
    glm::mat4 orthoP;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        obj._width = width;
        obj._height = height;
        obj.perspectiveP = glm::perspective(glm::radians(50.f), obj.getAspectRatio(), 0.1f, 1000.0f);
        if (obj.projectionMode == ProjectionMode::perspective)
            ShaderProgram::setMatrixP(obj.perspectiveP);
    }
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        switch (action)
        {
        case GLFW_PRESS:
            obj.keyQueue.push(key);
            break;

        case GLFW_RELEASE:
            obj.keyQueue.push(-key);
            break;
        }
    }
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        switch (action)
        {
        case GLFW_PRESS:
            obj.mouseButtonEventQueue.push({static_cast<MouseButton>(button + 1), true});
            break;

        case GLFW_RELEASE:
            obj.mouseButtonEventQueue.push({static_cast<MouseButton>(button + 1), false});
            break;
        }
    }
    static void mouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset)
    {
        Window &obj = *reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        obj.mouseScrollEventQueue.push({xOffset, yOffset});
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
    template <class T>
    static T popOrZero(std::queue<T> &q)
    {
        if (q.empty())
            return T();
        T r = q.front();
        q.pop();
        return r;
    }
};

#endif /* !RENDERER_H_ */
