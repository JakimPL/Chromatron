#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SFML/Graphics.hpp>

struct Graphics {
	std::vector<sf::Texture*> textures;
	std::vector<sf::Texture*> tiles;

	void deleteTextures();
	sf::Texture* loadTexture(const std::string &filename);
	void loadTextures();
};

#endif