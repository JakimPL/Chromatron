#include "auxiliary.h"
#include "main.h"

// Create a rectangle
sf::RectangleShape rectangleCreate(int x, int y, int w, int h, sf::Color color)
{
	sf::RectangleShape rectangle(sf::Vector2f(w, h));
	rectangle.setPosition(x, y);
	rectangle.setFillColor(color);
	return rectangle;
}

// Load a sprite
sf::Texture* loadTexture(const std::string &filename)
{
	std::string location = PATH_DATA + PATH_IMG_PREFIX + filename + PATH_IMG_SUFFIX;
	sf::Texture *image = new sf::Texture();
	if (!image->loadFromFile(location)) {
		throw std::runtime_error("failed to load " + location + " image file");
	}

	/*sf::Sprite sprite;
	sprite.setTexture(image);*/

	return image;
}

// Read a byte from a file
void readByte(std::ifstream *file, unsigned short &var)
{
	char buffer;
	file->read(&buffer, 1);
	var = static_cast<unsigned short>(buffer);
}