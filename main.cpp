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

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	sf::View view(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	window.setView(view);

	// Load a level
	game.loadLevel("000");
	game.editor.turn(editorOn);

	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			sf::Vector2f mousePositionVector = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			Object::Position mousePosition = Object::Position::createPosition(mousePositionVector);

			gameEvents(game, event, drag, mousePosition);
			window.clear();

			drawBoard(game, window, mousePosition);
			drawLasers(game, window);
			drawGameObject(game, window, drag, mousePosition);

			window.display();
		}
	}

	// Delete level objects
	deleteGameObjects(game);

	return 0;
}
