#include "Game.h"

int main()
{
    Window window(800, 600, "game");
    ShaderProgram::init();
    DrawableObject::init();
    DummyModel::init();
    Axes::init();

    Game game(window);
    game();

    DummyModel::fini();
    return 0;
}
