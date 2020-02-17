#include "game.hpp"

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
