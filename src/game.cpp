#include <fstream>

#include "game.hpp"
#include "auxiliary.hpp"
#include "log.hpp"

Game::Game()
{
	graphics.loadTextures();
	level.game = this;
	levelSet.game = this;
	editor.game = this;
}

Game::~Game()
{

}