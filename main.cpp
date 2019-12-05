#include <iostream>

#include "main.h"

int main(int argc, char* argv[])
{
	// Handle application's parameters
	bool editorOn = false;
	for (int arg = 0; arg < argc; ++arg) {
		editorOn = std::string(argv[arg]) == "--editor";
	}

	// Create a game object
	Game game;
	Drag drag;
	sf::Event event;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	window.setView(view);

	game.loadLevel("000");
	game.editor.turn(editorOn);

	GameState gameState(game, window, drag, event);

	mainLoop(gameState);
	deleteGameObjects(gameState);

	return 0;
}