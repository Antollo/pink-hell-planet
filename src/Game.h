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
#include "Skybox.h"
#include "Fireflies.h"
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
        clock60Pi.reset();
    }

    void operator()()
    {
        time = clock60Pi.getTime();
        ShaderProgram::setTime(time);
        if (time >= glm::pi<float>() * 60.f)
            clock60Pi.reset();

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
        window.draw(skybox);
        window.draw(fireflies);

        window.swapBuffers();

        static btSphereShape shape(20.f);
        shape.setMargin(0.f);
        static RigidBody bigHole(nullptr, static_cast<btCollisionShape *>(&shape), 0.f, glm::vec3(50.f, 20.f, 50.f));

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
        }
        if (player != nullptr)
        {
            switch (key)
            {
            case GLFW_KEY_W:
                player->goForward(true);
                break;
            case -GLFW_KEY_W:
                player->goForward(false);
                break;

            case GLFW_KEY_S:
                player->goBackward(true);
                break;
            case -GLFW_KEY_S:
                player->goBackward(false);
                break;

            case GLFW_KEY_A:
                player->goLeft(true);
                break;
            case -GLFW_KEY_A:
                player->goLeft(false);
                break;

            case GLFW_KEY_D:
                if (player != nullptr)
                    player->goRight(true);
                break;
            case -GLFW_KEY_D:
                player->goRight(false);
                break;

            case GLFW_KEY_SPACE:
                player->goUp(true);
                break;
            case -GLFW_KEY_SPACE:
                player->goUp(false);
                break;
            }
        }
    }

    Window &window;
    Clock clock, clock60Pi;
    World world;
    PlayableObject *player;
    Camera camera;
    Axes axes;
    Skybox skybox;
    Fireflies fireflies;
    float time, delta, maxDelta = 42.f;
    int key, frames = 0;
    std::vector<std::unique_ptr<DrawableObject>> drawableObjects;
    Terrain terrain;
};

#endif /* !GAME_H_ */
