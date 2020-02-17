#include "main.hpp"

int main(int argc, char* argv[])
{
	Log::log(Log::Type::NONE, "", true, false);

	Game game;
	Drag drag;
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	GameState gameState(game, window, drag, event);

	Object::createObjectMap();

	gameState.handleApplicationParameters(argc, argv);
	gameState.initializeGame();
	gameState.mainLoop();
	gameState.endGame();

	return 0;
}
