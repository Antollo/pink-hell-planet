#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <cmath>

#include "Axes.h"
#include "Camera.h"
#include "Clock.h"
#include "debug.h"
#include "DummyModel.h"
#include "Terrain.h"
#include "Window.h"
#include "World.h"

class Game
{
public:
    Game(Window &w) : window(w), player(nullptr), camera(w, player), terrain(world)
    {
        drawableObjects.push_back(std::make_unique<DummyModel>(world));
        drawableObjects.push_back(std::make_unique<DummyModel>(world));
        drawableObjects.push_back(std::make_unique<DummyModel>(world));

        player = dynamic_cast<PlayableObject *>(drawableObjects.front().get());
        clock.reset();
    }

    void operator()()
    {
        frames++;
        time = clock.getTime();
        delta = clock.getDelta();
        if (time >= 2.f)
        {
            clock.reset();
            std::cerr << "fps: " << frames / 2 << std::endl;
            frames = 0;
            std::cerr << "player position: " << player->getPosition() << std::endl;
        }

        while (key = window.pollKey())
            consumeKey(key);

        if (time < 1.f)
            window.setClearColor(0.5f * time, time, 1.f - time);
        else
            window.setClearColor(0.5f - 0.5f * time, 2.f - time, time - 1.f);

        camera.update(delta);
        world.update(delta);
        for (auto &objectPtr : drawableObjects)
            objectPtr->update(delta);

        window.clear();
        window.draw(axes);
        for (auto &objectPtr : drawableObjects)
            window.draw(*objectPtr);

        terrain.updateBuffers();
        window.draw(terrain);

        window.swapBuffers();
    }

private:
    void consumeKey(int key)
    {
        camera.consumeKey(key);
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            window.close();
            break;

        case GLFW_KEY_W:
            if (player != nullptr)
                player->goForward(true);
            break;
        case -GLFW_KEY_W:
            if (player != nullptr)
                player->goForward(false);
            break;

        case GLFW_KEY_S:
            if (player != nullptr)
                player->goBackward(true);
            break;
        case -GLFW_KEY_S:
            if (player != nullptr)
                player->goBackward(false);
            break;

        case GLFW_KEY_A:
            if (player != nullptr)
                player->goLeft(true);
            break;
        case -GLFW_KEY_A:
            if (player != nullptr)
                player->goLeft(false);
            break;

        case GLFW_KEY_D:
            if (player != nullptr)
                player->goRight(true);
            break;
        case -GLFW_KEY_D:
            if (player != nullptr)
                player->goRight(false);
            break;

        case GLFW_KEY_SPACE:
            if (player != nullptr)
                player->goUp(true);
            break;
        case -GLFW_KEY_SPACE:
            if (player != nullptr)
                player->goUp(false);
            break;
        }
    }

    Clock clock;
    Camera camera;
    Window &window;
    World world;
    Axes axes;
    float time, delta;
    int key, frames = 0;
    std::vector<std::unique_ptr<DrawableObject>> drawableObjects;
    Terrain terrain;
    PlayableObject *player;
};

#endif /* !GAME_H_ */
