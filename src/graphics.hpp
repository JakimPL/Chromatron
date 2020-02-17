#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>

struct Graphics {
	std::vector<sf::Texture*> textures;
	std::vector<sf::Texture*> additionalTextures;
	std::vector<sf::Texture*> tiles;
	sf::Font font;

	void deleteTextures();
	sf::Texture* loadTexture(const std::string &filename);
	void loadTextures();
	void loadFont();
};

#endif
