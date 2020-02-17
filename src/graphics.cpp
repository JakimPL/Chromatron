#include <fstream>
#include <SFML/Graphics.hpp>
#include "graphics.hpp"
#include "constants.hpp"
#include "log.hpp"

sf::Texture* Graphics::loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture* image = new sf::Texture();
	if (filename != "") {
		if (!image->loadFromFile(location)) {
			LogError("Failed to load " + location + " image file");
			throw std::runtime_error("failed to load " + location + " image file");
		}
	}

	return image;
}

void Graphics::loadTextures()
{
	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		textures.push_back(loadTexture(IMG_OBJECT_NAMES[index]));
	}

	for (size_t index = 0; index < OBJ_COUNT; ++index) {
		additionalTextures.push_back(loadTexture(IMG_EXTRA_OBJECT_NAMES[index]));
	}

	for (size_t index = 0; index < TIL_COUNT; ++index) {
		tiles.push_back(loadTexture(IMG_TILE_NAMES[index]));
	}
}

void Graphics::deleteTextures()
{
	for (size_t index = 0; index < textures.size(); ++index) {
		delete textures[index];
	}

	for (size_t index = 0; index < tiles.size(); ++index) {
		delete tiles[index];
	}
}

void Graphics::loadFont()
{
	std::string location = PATH_DATA + PATH_FNT;
	if (!font.loadFromFile(location)) {
		LogError("Failed to load " + location + " font file");
		throw std::runtime_error("Failed to load " + location + " font file");
	} else {
		LogInfo("Font " + location + " loaded");
	}
}
