#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <iomanip>
#include <cmath>

#include "Axes.h"
#include "Bullet.h"
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
#include "ParticleSystem.h"
#include "Text.h"
#include "Crosshair.h"
#include "Bot.h"

class Game
{
public:
    Game(Window &w) : window(w), player(nullptr), camera(w, player), terrain(world), crosshair(camera)
    {
        // drawableObjects.push_back(std::make_unique<DummyModel>(world));
        // drawableObjects.push_back(std::make_unique<DummyModel>(world));
        // drawableObjects.push_back(std::make_unique<DummyModel>(world));

        auto newPlayer = std::make_shared<DummyModel>(world);
        player = newPlayer.get();
        drawableObjects.push_back(newPlayer);

        auto newBot = std::make_shared<Bot>(world);
        newBot->target(newPlayer);
        drawableObjects.push_back(newBot);

        drawableObjects.emplace_back(new Bullet(world));

        clock.reset();
        clock60Pi.reset();
        fpsText.setPosition({-0.95f, 0.9f});
        fpsText.setColor({1.f, 1.f, 1.f, 0.5f});
    }

    void operator()()
    {
        time = clock60Pi.getTime();
        ShaderProgram::setTime(time);
        particleSystem.update(time);

        if (time >= glm::pi<float>() * 60.f)
            clock60Pi.reset();

        frames++;
        time = clock.getTime();
        delta = clock.getDelta();
        fps = (9.f * fps + 1.f / delta) / 10.f;
        fpsText.setText("fps: " + std::to_string(int(fps + 0.5f)));
        if (time >= 2.f)
        {
            clock.reset();
            std::cerr << "fps: " << frames / 2 << std::endl;
            std::cerr << "worst frametime in last 2s: " << std::setprecision(5) << maxDelta << " (" << 1 / maxDelta << " fps)" << std::endl;
            maxDelta = delta;
            frames = 0;
        }
        maxDelta = std::max(maxDelta, delta);

        while (int key = window.pollKey())
            consumeKey(key);
        while (int button = window.pollMouseButton())
            consumeButton(button);

        float xDiff, yDiff;
        window.getCursorDiff(xDiff, yDiff);
        camera.consumeCursorDiff(xDiff, yDiff);
        if (player != nullptr)
            player->consumeCursorDiff(xDiff, yDiff);

        if (time < 1.f)
            window.setClearColor(0.5f * time, time, 1.f - time);
        else
            window.setClearColor(0.5f - 0.5f * time, 2.f - time, time - 1.f);

        camera.update(delta);

        for (int i = 0; i < 10; i++)
            world.update(delta / 10);

        for (auto &objectPtr : drawableObjects)
            objectPtr->update(delta);

        if (testExplosionClock.getTime() >= 6.f)
        {
            testExplosionClock.reset();
            particleSystem.generate(glm::vec3(glm::linearRand(0.f, 80.f), 10.f, glm::linearRand(0.f, 80.f)));
        }

        crosshair.update();

        GuiObject::setAspectRatioAndScale(window.getAspectRatio(), 1.f / window.getWidth());

        window.clear();

        terrain.updateBuffers();

        for (auto &objectPtr : drawableObjects)
            if (objectPtr.get() != player)
                window.draw(*objectPtr);

        window.draw(terrain);
        window.draw(skybox);
        window.draw(fireflies);
        window.draw(particleSystem);
        window.draw(*player);
        window.draw(fpsText);
        window.draw(crosshair);

        window.swapBuffers();

        static btSphereShape shape(20.f);
        shape.setMargin(0.f);
        static RigidBody bigHole(nullptr, static_cast<btCollisionShape *>(&shape), 0.f, glm::vec3(50.f, 20.f, 50.f));

        if (time > 2.f)
            terrain.collideWith(bigHole);
    }

private:
    void consumeKey(int glfwKeyCode)
    {
        switch (glfwKeyCode)
        {
        case GLFW_KEY_ESCAPE:
            window.close();
            break;
        }

        if (player != nullptr)
        {
            player->consumeKey(glfwKeyCode);
        }
        else
            camera.consumeKey(glfwKeyCode);
    }
    void consumeButton(int glfwButtonCode)
    {
        if (player != nullptr)
            player->consumeButton(glfwButtonCode);
    }

    Window &window;
    Clock clock, clock60Pi, testExplosionClock;
    Text fpsText;
    World world;
    PlayableObject *player;
    Camera camera;
    Terrain terrain;
    Crosshair crosshair;
    Skybox skybox;
    Fireflies fireflies;
    ParticleSystem particleSystem;
    float time, delta, maxDelta = 42.f, fps;
    int frames = 0;
    std::vector<std::shared_ptr<DrawableObject>> drawableObjects;
};

#endif /* !GAME_H_ */
