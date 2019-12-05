#include "auxiliary.h"
#include "gamestate.h"

GameState::GameState(Game &gam, sf::RenderWindow &win, Drag &drg, sf::Event &ev) : game(gam), window(win), drag(drg), event(ev)
{

}

void GameState::update()
{
	sf::Event ev;
	event = ev;

	mousePosition = floatToPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
}