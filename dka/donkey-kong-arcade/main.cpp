/*
 * Exemplo de uso do framework de jogo
 *
 * Cria um jogo com 3 estados: "menu", "play" e "pause"
 *
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cmath>

#include "Game.h"
#include "MenuState.h"

int main(int argc, char **argv)
{
    cgf::Game game(5,30);

	game.init("Donkey Kong",800,800,false);

    game.changeState(MenuState::instance());

	while(game.isRunning())
	{
		game.handleEvents();
		game.update();
		game.draw();
	}

	// cleanup the engine
	game.clean();

    return 0;
}
