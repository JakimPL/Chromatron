#include <iostream>

#include "main.h"

int main(int argc, char* argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Create a game object
	Game game;

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");

	// Load a level
	game.loadLevel("000");

	bool gameEvent = true;

	// Game main loop
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}

			window.clear();

			if (gameEvent) {
				game.calculateLasers();
				game.updateDots();
				gameEvent = false;
			}

			// Draw the board
			for (short y = 0; y < game.level.height; ++y) {
				for (short x = 0; x < game.level.width; ++x) {
					sf::Color outlineColor = (game.level.obstacles[x][y] ? lgray : dgray);
					sf::Color fillColor = (game.level.obstacles[x][y] ? dgray : gray);
					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x, OFFSET_Y + TILE_SIZE * y, TILE_SIZE, TILE_SIZE, outlineColor));
					window.draw(rectangleCreate(OFFSET_X + TILE_SIZE * x + OUTLINE_SIZE, OFFSET_Y + TILE_SIZE * y + OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, TILE_SIZE - 2 * OUTLINE_SIZE, fillColor));
				}
			}


			// Draw lasers
			for (size_t beamerIndex = 0; beamerIndex < game.level.objectList[OBJ_BEAMER].size(); ++beamerIndex) {
				Beamer* beamer = (Beamer*) game.level.objectList[OBJ_BEAMER][beamerIndex];
				for (size_t rayIndex = 0; rayIndex < beamer->laser.size(); ++rayIndex) {
					Ray ray = beamer->laser[rayIndex];
					sf::Vertex line[2];
					line[0].position = sf::Vector2f(ray.x_start, ray.y_start);
					line[1].position = sf::Vector2f(ray.x_end, ray.y_end);
					line[0].color = line[1].color = ray.color.convertToColor();
					window.draw(line, 2, sf::Lines);
				}
			}

			// Draw game objects
			for (size_t type = 0; type < OBJ_COUNT; ++type) {
				for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
					// Draw the sprite
					window.draw(game.level.objectList[type][index]->sprite);
				}
			}

			window.display();
		}
	}

	// Delete level objects
	for (size_t type = 0; type < OBJ_COUNT; ++type) {
		for (size_t index = 0; index < game.level.objectList[type].size(); ++index) {
			delete game.level.objectList[type][index];
		}
	}

	return 0;
}
