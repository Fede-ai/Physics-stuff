#include "wall.h"

Wall::Wall(sf::RectangleShape rect, char inType)
{
	body = rect;
	type = inType;

	if (inType == 'r')
		body.setFillColor(sf::Color(0, 110, 80));
	else if (inType == 'n')
		body.setFillColor(sf::Color(200, 50, 50));
}

void Wall::draw(sf::RenderWindow& window)
{
	window.draw(body);
}
