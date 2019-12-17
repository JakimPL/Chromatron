#include "main.h"

int main(int argc, char* argv[])
{
	Log::log(Log::Type::NONE, "", true, false);

	Game game;
	Drag drag;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	GameState gameState(game, window, drag, event);

	LogInfo("Game objects initialized");

	gameState.handleApplicationParameters(argc, argv);
	gameState.initializeGame();

	LogNone("Game starts");

	gameState.mainLoop();
	gameState.endGame();

	LogNone("Game ends");

	return 0;
}