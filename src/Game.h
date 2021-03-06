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
#include "Menu.h"
#include "Music.h"
#include "DamageOverlay.h"
#include "ExplosionList.h"

class Game
{
public:
    static Game *get()
    {
        return ptr.get();
    }

    static Terrain *getTerrain()
    {
        return &ptr->terrain;
    }

    static ParticleSystem *getParticleSystem()
    {
        return &ptr->particleSystem;
    }

    static const glm::vec3 &getCameraPosition()
    {
        return ptr->camera.getPosition();
    }

    static void init(Window &window, GlobalConfig &config)
    {
        ptr.reset(new Game(window, config));
    }

    static void addDrawable(std::unique_ptr<DrawableObject> &&drawable)
    {
        ptr->drawableObjects.emplace_back(std::move(drawable));
    }

    void tick()
    {
        GuiObject::setAspectRatioGlobalScale(window.getAspectRatio(), 1.f / window.getWidth());
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

        while (int key = window.pollKeyEvent())
            consumeKeyEvent(key);
        while (auto ev = window.pollMouseButtonEvent())
            consumeMouseButtonEvent(ev);
        while (auto ev = window.pollMouseScrollEvent())
            consumeMouseScrollEvent(ev);

        float xDiff, yDiff;
        window.getCursorDiff(xDiff, yDiff);
        camera.consumeCursorDiff(xDiff, yDiff);
        if (player != nullptr)
            player->consumeCursorDiff(xDiff, yDiff);

        window.setClearColor(0.1, 0.4, 0.1);

        terrain.update();

        for (auto &i : drawableObjects)
            i->update(delta);

        camera.update(delta);
        damageOverlay.update(delta);
        crosshair.update(delta);
        menu.update(delta);

        auto it = drawableObjects.begin();
        while (it != drawableObjects.end())
        {
            if ((*it)->isAlive())
                it++;
            else
            {
                static RigidBody *ptr;
                ptr = dynamic_cast<RigidBody *>((*it).get());

                if (ptr == player)
                {
                    MusicManager::get("failure.wav")
                        .setVolumeMultiplier(4.f)
                        .elevate()
                        .playDelayed(1.f);
                    player = nullptr;
                }
                if (dynamic_cast<Bullet *>(ptr) != nullptr)
                    MusicManager::get("explosion.wav")
                        .setVolumeMultiplier(Music::volumeMultiplier(15.f, getCameraPosition() - ptr->getPosition()))
                        .play();
                else if (dynamic_cast<Bot *>(ptr) != nullptr)
                    MusicManager::get("success.wav")
                        .setVolumeMultiplier(4.f)
                        .elevate()
                        .playDelayed(1.f);

                it = drawableObjects.erase(it);
            }
        }

        for (auto &i : drawableObjects)
            i->update(delta);

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
        window.draw(damageOverlay);
        window.draw(fpsText);
        window.draw(crosshair);
        window.draw(menu);

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

    Game(Window &w, GlobalConfig& config)
        : window(w), globalConfig(config), player(nullptr), camera(w, player), damageOverlay(player),
          terrain(world), crosshair(camera), backgroundMusic("music.wav"), fps(0.f)
    {
        btSphereShape *explosionShape = new btSphereShape(5);
        for (auto& i : explosionList::dummy)
        {
            GhostObject *ghostObject = new GhostObject(nullptr, explosionShape, i);
            terrain.collideWith(std::unique_ptr<CollisionObject>(ghostObject));
        }
        terrain.forceFullUpdate();

        clock.reset();
        clock60Pi.reset();
        fpsText.setPosition({-1.f, 1.f});
        fpsText.setPositionOffset({20.f, -20.f - Text::glyphHeight});
        fpsText.setColor({1.f, 1.f, 1.f, 0.5f});

        menu.insert(Action(
            GLFW_KEY_P,
            "P - Play game",
            [this]() {
                auto newPlayer = std::make_shared<DummyModel>(world);
                player = newPlayer.get();
                drawableObjects.push_back(newPlayer);

                auto newBot = std::make_shared<Bot>(world);
                newBot->target(newPlayer);
                drawableObjects.push_back(newBot);
            },
            [this]() { return player == nullptr; }));

        menu.insert(Action(
            GLFW_KEY_G,
            "G - Go up",
            [this]() {
                glm::vec3 position = player->getPosition();
                position.y = 20.f;
                player->setPosition(position);
            },
            [this]() { return player != nullptr && player->getPosition().y < 0.f; }));
        menu.insert(Action(
            GLFW_KEY_F,
            "F - Toggle fullscreen",
            [this]() {
                window.toggleFullscreen();
                globalConfig.setFullscreen(!globalConfig.getFullscreen());
            },
            [this]() { return player == nullptr; }));
        menu.insert(Action(
            GLFW_KEY_L,
            "L - Lower quality",
            [this]() {
                globalConfig.setGraphicSetting(GlobalConfig::GraphicSetting::low);
                globalConfig.flush();
                restartGame();
            },
            [this]() { return player == nullptr && globalConfig.getGraphicSetting() != GlobalConfig::GraphicSetting::low; }));
        menu.insert(Action(
            GLFW_KEY_H,
            "H - Higher quality",
            [this]() {
                globalConfig.setGraphicSetting(GlobalConfig::GraphicSetting::normal);
                globalConfig.flush();
                restartGame();
            },
            [this]() { return player == nullptr && globalConfig.getGraphicSetting() != GlobalConfig::GraphicSetting::normal; }));

        running = true;
        mainReady = true;

        physicsThread = std::async(std::launch::async, [this] {
            while (running)
            {
                std::unique_lock<std::mutex> lk(mutex);
                while (mainReady)
                    cv.wait(lk);

                for (int i = 0; i < physicsFramesPerGraphic; i++)
                    world.update(delta / physicsFramesPerGraphic);

                mainReady = true;
                cv.notify_one();
            }
        });

        backgroundMusic.setLoop().play();

        if (globalConfig.getFullscreen())
            window.toggleFullscreen();

        window.focus();
    }

    friend class std::condition_variable;

    void consumeKeyEvent(int glfwKeyCode)
    {
        if (glfwKeyCode == GLFW_KEY_ESCAPE)
            window.close();

        if (player != nullptr)
            player->consumeKeyEvent(glfwKeyCode);

        camera.consumeKeyEvent(glfwKeyCode);
        menu.consumeKeyEvent(glfwKeyCode);
    }

    void consumeMouseButtonEvent(Window::MouseButtonEvent ev)
    {
        if (player != nullptr)
            player->consumeMouseButtonEvent(ev);
    }

    void consumeMouseScrollEvent(Window::MouseScrollEvent ev)
    {
        camera.consumeMouseScrollEvent(ev);
    }

    void restartGame()
    {
        #ifdef _WIN32
        std::system("start /b game");
        #else
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wunused-result"
        std::system("./game &");
        #pragma GCC diagnostic pop
        #endif
        window.close();
    }

    Window &window;
    GlobalConfig& globalConfig;
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
    DamageOverlay damageOverlay;
    Menu menu;
    Music backgroundMusic;
    float time, delta, maxDelta = 42.f, fps;
    int frames = 0;
    std::list<std::shared_ptr<DrawableObject>> drawableObjects;

    std::future<void> physicsThread;
    std::atomic<bool> running, mainReady;
    std::condition_variable cv;
    std::mutex mutex;

    static inline constexpr int physicsFramesPerGraphic = 10;
};

#endif /* !GAME_H_ */
