#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <iomanip>
#include <cmath>

#include "Axes.h"
#include "Camera.h"
#include "Clock.h"
#include "debug.h"
#include "DummyModel.h"
#include "GhostObject.h"
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
            std::cerr << "worst frametime in last 2s: " << std::setprecision(5) << maxDelta << " (" << 1 / maxDelta << " fps)" << std::endl;
            maxDelta = delta;
            frames = 0;
            std::cerr << "player position: " << player->getPosition() << std::endl;
        }
        maxDelta = std::max(maxDelta, delta);

        while ((key = window.pollKey()))
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

        static btSphereShape shape(20.f);
        shape.setMargin(0.f);
        static RigidBody bigHole(nullptr, static_cast<btCollisionShape*>(&shape), 0.f, glm::vec3(50.f, 20.f, 50.f));

        if (time > 2.f)
            terrain.collideWith(bigHole);
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

    Window &window;
    Clock clock;
    World world;
    PlayableObject *player;
    Camera camera;
    Axes axes;
    float time, delta, maxDelta = 42.f;
    int key, frames = 0;
    std::vector<std::unique_ptr<DrawableObject>> drawableObjects;
    Terrain terrain;
};

#endif /* !GAME_H_ */
