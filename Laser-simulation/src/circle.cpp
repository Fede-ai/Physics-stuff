#include "circle.h"

Circle::Circle(sf::CircleShape circle, char inType)
{
	body = circle;

	if (inType == 's')
	{
		body.setFillColor(sf::Color::Green);
		isReflective = true;
	}
	else if (inType == 'f')
	{
		body.setFillColor(sf::Color::Red);
		isReflective = false;
	}
}

void Circle::draw(sf::RenderWindow& window)
{
	window.draw(body);
}

bool Circle::reflects()
{
	return isReflective;
}
