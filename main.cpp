#include <iostream>

#include "main.h"

int main(int argc, char *argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");

	// Load a level
	Level level("000");
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
					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x, OFFSET_Y + TILE_SIZE * y, TILE_SIZE, TILE_SIZE, outlineColor));
					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x + OUTLINE_SIZE, OFFSET_Y + TILE_SIZE * y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
				}
			}
			window.display();
		}
	}

	// Delete level objects
	size_t objectsCount = level.objectList.size();
	for (size_t index = 0; index < objectsCount; ++index) {
		delete level.objectList[index];
	}

	return 0;
}
