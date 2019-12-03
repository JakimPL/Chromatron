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

			Object::Position mousePosition = Object::Position::createPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
			gameEvents(game, event, drag, mousePosition);
			draw(game, window, drag, mousePosition);
		}
	}

	// Delete level objects
	deleteGameObjects(game);

	return 0;
}
