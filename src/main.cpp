#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

template <class... Args>
void error[[noreturn]](Args... args)
{
    // C++17 fold expression <3
    (std::cerr << ... << args) << std::endl;
    glfwTerminate();
    std::exit(-1);
}

int main()
{
    float time;

    if (!glfwInit())
        error("glfwInit failed");

    glfwSetErrorCallback([](int error, const char *description) {
        std::cerr << "error: " << description << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow *window(glfwCreateWindow(200, 200, "sample window", nullptr, nullptr));
    if (!window)
        error("glfwCreateWindow failed");

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
        error("gladLoadGL failed");

    while (!glfwWindowShouldClose(window))
    {
        time = glfwGetTime();
        if (time >= 2.f)
            glfwSetTime(0.f);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (time < 1.f)
            glClearColor(1.f - time, time, 0.5f * time, 1.0f);
        else
            glClearColor(time - 1.f, 2.f - time, 0.5f - 0.5f * time, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}