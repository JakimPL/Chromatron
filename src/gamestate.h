#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include "game.h"

struct GameState {
	GameState(Game &gam, sf::RenderWindow &win, Drag &drg, sf::Event &ev);
	Game &game;
	sf::RenderWindow &window;
	Drag &drag;
	sf::Event &event;

	Object::Position mousePosition;
	void update();
};

#endif