#include "Game.h"

#ifdef _WIN32
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
extern "C" __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance = 1;
#endif

int main()
{
    Window window(800, 600, "game");
    ShaderProgram::init();
    DrawableObject::init();
    DummyModel::init();
    Axes::init();
    Terrain::init();
    Skybox::init();
    Fireflies::init();
    ParticleSystem::init();

    Game game(window);
    while (window.isOpen())
        game();

    return 0;
}
