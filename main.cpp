#include <iostream>

#include "main.h"

int main(int argc, char *argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Create a game object
	Game game;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(game.SCREEN_WIDTH, game.SCREEN_HEIGHT), "Chromatron");

	// Load a level
	Level level(&game, "000");

	// Game main loop
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			window.clear();

			// Draw the board
			for (short y = 0; y < level.height; ++y) {
				for (short x = 0; x < level.width; ++x) {
					sf::Color outlineColor = (level.obstacles[x][y] ? lgray : dgray);
					sf::Color fillColor = (level.obstacles[x][y] ? dgray : gray);
					window.draw(rectangleCreate(game.OFFSET_X + game.TILE_SIZE * x, game.OFFSET_Y + game.TILE_SIZE * y, game.TILE_SIZE, game.TILE_SIZE, outlineColor));
					window.draw(rectangleCreate(game.OFFSET_X + game.TILE_SIZE * x + game.OUTLINE_SIZE, game.OFFSET_Y + game.TILE_SIZE * y + game.OUTLINE_SIZE, game.TILE_SIZE - 2 * game.OUTLINE_SIZE, game.TILE_SIZE - 2 * game.OUTLINE_SIZE, fillColor));
				}
			}

			// Draw game objects
			size_t objectsCount = level.objectList.size();
			for (size_t index = 0; index < objectsCount; ++index) {
				///TODO: draw sprites
				window.draw(level.objectList[index]->sprite);
			}

			window.display();
		}
	}

	// Delete level objects
	for (size_t index = 0; index < level.objectList.size(); ++index) {
		delete level.objectList[index];
	}

	return 0;
}
