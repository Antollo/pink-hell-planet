#ifndef CLOCK_H_
#define CLOCK_H_

#include <GLFW/glfw3.h>

class Clock
{
public:
    Clock() : lastTime(0.0), resetTime(0.0) {}
    float getDelta()
    {
        double time = getTime();
        float delta = time - lastTime;
        lastTime = time;
        return delta;
    }
    float getTime() const { return glfwGetTime() - resetTime; }
    void reset()
    {
        resetTime = resetTime + getTime();
        lastTime = 0.0;
    }

private:
    double lastTime;
    double resetTime;
};

#endif /* !CLOCK_H_ */
