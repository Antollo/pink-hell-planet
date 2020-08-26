#ifndef GAME_H_
#define GAME_H_

#include <iostream>
#include <iomanip>
#include <cmath>
#include <future>
#include <atomic>
#include <condition_variable>
#include <thread>

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
    static Game* get()
    {
        return ptr.get();
    }

    static Terrain* getTerrain()
    {
        return &ptr->terrain;
    }

    static ParticleSystem* getParticleSystem()
    {
        return &ptr->particleSystem;
    }

    static void init(Window &window)
    {
        ptr.reset(new Game(window));
    }

    static void addDrawable(std::unique_ptr<DrawableObject>&& drawable)
    {
        ptr->drawableObjects.emplace_back(std::move(drawable));
    }

    void tick()
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
        while (auto ev = window.pollMouseButton())
            consumeButton(ev);

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

        for (auto it = drawableObjects.begin(); it != drawableObjects.end(); it++)
            (*it)->update(delta);

        crosshair.update();

        GuiObject::setAspectRatioAndScale(window.getAspectRatio(), 1.f / window.getWidth());

        terrain.updateBuffers();

        auto it = drawableObjects.begin();
        while (it != drawableObjects.end())
        {
            if ((*it)->isAlive())
                it++;
            else
                it = drawableObjects.erase(it);
        }

        mainReady = false;
        cv.notify_one();

        window.clear();

        for (auto &objectPtr : drawableObjects)
            if (objectPtr.get() != player)
                window.draw(*objectPtr);

        window.draw(terrain);
        window.draw(skybox);
        window.draw(fireflies);
        window.draw(particleSystem);
        if (player != nullptr)
            window.draw(*player);
        window.draw(fpsText);
        window.draw(crosshair);

        window.swapBuffers();

        std::unique_lock<std::mutex> lk(mutex);
        while (!mainReady)
            cv.wait(lk);
    }

    ~Game()
    {
        mainReady = false;
        running = false;
        cv.notify_one();
        if (physicsThread.valid())
            physicsThread.get();
    }

private:
    static inline std::unique_ptr<Game> ptr;

    Game(Window &w) : window(w), player(nullptr), camera(w, player), terrain(world), crosshair(camera)
    {
        auto newPlayer = std::make_shared<DummyModel>(world);
        player = newPlayer.get();
        drawableObjects.push_back(newPlayer);

        auto newBot = std::make_shared<Bot>(world);
        newBot->target(newPlayer);
        drawableObjects.push_back(newBot);

        //TODO temporary while player dying is not handled
        newPlayer->setHP(111222333);

        clock.reset();
        clock60Pi.reset();
        fpsText.setPosition({-0.95f, 0.9f});
        fpsText.setColor({1.f, 1.f, 1.f, 0.5f});

        running = true;
        mainReady = true;

        physicsThread = std::async(std::launch::async, [this] {
            while (running)
            {
                std::unique_lock<std::mutex> lk(mutex);
                while (mainReady)
                    cv.wait(lk);

                for (int i = 0; i < 10; i++)
                    world.update(delta / 10);

                mainReady = true;
                cv.notify_one();
            }
        });

        particleSystem.generate(glm::vec3({10, 10, 10}));
    }

    friend class std::condition_variable;
    void consumeKey(int glfwKeyCode)
    {
        switch (glfwKeyCode)
        {
        case GLFW_KEY_ESCAPE:
            window.close();
            break;
        }

        if (player != nullptr)
            player->consumeKey(glfwKeyCode);
        else
            camera.consumeKey(glfwKeyCode);
    }

    void consumeButton(Window::MouseButtonEvent ev)
    {
        if (player != nullptr)
            player->consumeButton(ev);
    }

    Window &window;
    Clock clock, clock60Pi;
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
    std::list<std::shared_ptr<DrawableObject>> drawableObjects;

    std::future<void> physicsThread;
    std::atomic<bool> running, mainReady;
    std::condition_variable cv;
    std::mutex mutex;
};

#endif /* !GAME_H_ */
