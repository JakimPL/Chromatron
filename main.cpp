#include "main.h"

int main(int argc, char* argv[])
{
	Game game;
	Drag drag;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	GameState gameState(game, window, drag, event);

	handleApplicationParameters(gameState, argc, argv);
	initializeGame(gameState);
	mainLoop(gameState);
	deleteGameObjects(gameState);

	return 0;
}