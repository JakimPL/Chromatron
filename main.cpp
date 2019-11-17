#include "main.h"

int main(int argc, char *argv[])
{
	// Handle application's parameters
	for (int arg = 0; arg < argc; ++arg) {
		///TODO: Handle application's parameters
	}

	// Initialize the window
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chromatron");
	Level level;
	// Game main loop
	while (window.isOpen()) {
		Ev event;
		while (window.pollEvent(event)) {
			if (event.type == Ev::Closed or event.key.code == Key::Escape) {
				window.close();
			}
		}

		///TODO: drawing sprites
	}

	return 0;
}
