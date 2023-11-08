#include <SFML/Graphics.hpp>
#include "wall.h"
#include "laser.h"

class Simulation
{
public:
	Simulation();
	int run();

private:
	void move();
	void draw();

	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::Vector2f viewSize = sf::Vector2f(1920, 1080);
	sf::Vector2f lastMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	
	sf::VertexArray background;
	sf::RectangleShape building;

	char buildingType = ' ';
	bool isBuilding = false;
	bool canSelectBuilding = false;
	float wallThickness = 4;
	bool clickEvent = false;
	bool updateNeeded = true;

	std::vector<Wall> walls;
	std::vector<Laser> lasers;
};