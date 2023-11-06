#include <SFML/Graphics.hpp>

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
};