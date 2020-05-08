#include "Game.h"

#include "VecInt3.h"
#include "Terrain.h"

int main()
{
    Window window(800, 600, "game");
    ShaderProgram::init();
    DrawableObject::init();
    DummyModel::init();
    Axes::init();

    Game game(window);
    while (window.isOpen())
        game();

    return 0;
}
