#ifndef CLOCK_H_
#define CLOCK_H_

class Clock
{
public:
    Clock() : lastTime(0.0), resetTime(0.0) {}
    float getDelta()
    {
        double time = getTime();
        double delta = time - lastTime;
        lastTime = time;
        return delta;
    }
    float getTime() { return glfwGetTime() - resetTime; }
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
