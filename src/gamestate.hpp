#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SFML/Graphics.hpp>
#include "game.hpp"

struct GameState {
	GameState(Game &gam, sf::RenderWindow &win, Drag &drg, sf::Event &ev);
	Game &game;
	sf::RenderWindow &window;
	Drag &drag;
	sf::Event &event;
	Position mousePosition;

	void handleApplicationParameters(int argc, char* argv[]);
	void clearLevel();
	void deleteGameObjects();
	void draw();
	void drawBoard();
	void drawLasers(bool blackLasers);
	void drawLasers();
	void drawGameObjects();
	void drawSelector(Position mousePosition);
	void drawSelectorSquare(Position position, sf::Color fillColor, sf::Color outlineColor, bool outlineOnly);
	void drawStack();
	void drawTile(Position position, bool inStack = false);
	void endGame();
	void gameEvents();
	void initializeGame();
	void keyboardGlobalEvents();
	void keyboardEditorEvents();
	void loadLevel();
	void mainLoop();
	void mouseEditorEvents();
	void mouseGameEvents();
	void nextLevel();
	void previousLevel();
	void resetLevel();
	void saveCurrentLevel();
	void saveLevel();
	void update();
};

#endif