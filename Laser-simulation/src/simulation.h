#include <SFML/Graphics.hpp>
#include "block.h"

class Simulation
{
public:
	Simulation();
	int run();

private:
	void update();
	void draw();

	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::Vector2f viewSize = sf::Vector2f(1920, 1080);
	sf::Vector2f lastMousePos = window.mapPixelToCoords(sf::Mouse::getPosition());

	Block laser1;
};