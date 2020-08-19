
#include "Game.h"
#include "GlobalConfig.hpp"


#ifdef _WIN32
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
#endif

int main()
{
    GlobalConfig config("config.json");

    Window window(config.getResX(), config.getResY(), "game");
    ShaderProgram::init(config.getGraphicSetting());
    DrawableObject::init();
    DummyModel::init();
    Axes::init();
    Terrain::init();
    Skybox::init();
    Fireflies::init();
    ParticleSystem::init(config.getGraphicSetting());
    GuiObject::init();
    Text::init();
    Bullet::init();

    Game game(window);
    while (window.isOpen())
        game();

    return 0;
}
