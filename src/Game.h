#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <cmath>
#include "Window.h"
#include "DummyModel.h"
#include "Axes.h"
#include "World.h"
#include "Clock.h"
#include "Camera.h"

class Game
{
public:
    Game(Window &w) : window(w), camera(w) {}

    void operator()()
    {
        objects.push_back(std::make_unique<DummyModel>(world));
        objects.push_back(std::make_unique<DummyModel>(world));
        objects.push_back(std::make_unique<DummyModel>(world));
        
        clock.reset();
        while (window.isOpen())
        {
            frames++;
            time = clock.getTime();
            delta = clock.getDelta();
            if (time >= 1.f)
            {
                clock.reset();
                std::cerr << "fps: " << frames << std::endl;
                frames = 0;
            }

            while (key = window.pollKey())
            {
                camera.consumeKey(key);
                switch (key)
                {
                case GLFW_KEY_ESCAPE:
                    window.close();
                    break;
                }
            }

            if (time < 1.f)
                window.setClearColor(0.5f * time, time, 1.f - time);
            else
                window.setClearColor(0.5f - 0.5f * time, 2.f - time, time - 1.f);

            camera.update(delta);
            world.update(delta);
            for (auto &objectPtr : objects)
                objectPtr->update(delta);

            window.clear();
            window.draw(axes);
            for (auto &objectPtr : objects)
                window.draw(*objectPtr);

            window.swapBuffers();
        }
    }

private:
    Clock clock;
    Camera camera;
    Window &window;
    World world;
    Axes axes;
    float time, delta;
    int key, frames = 0;
    std::vector<std::unique_ptr<DrawableObject>> objects;
};

#endif /* !GAME_H_ */
