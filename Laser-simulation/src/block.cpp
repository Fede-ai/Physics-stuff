#include "block.h"
constexpr auto PI = 3.14159265358979323846;

Block::Block(sf::RectangleShape rect, char inType)
{
	body = rect;

	if (inType == 'r')
	{
		body.setFillColor(sf::Color::Green);
		isReflective = true;
	}
	else if (inType == 'n')
	{
		body.setFillColor(sf::Color::Red);
		isReflective = false;
	}

	p1 = rect.getPosition();
	p2 = rect.getPosition();
	p2.x += cos(rect.getRotation() * PI / 180) * rect.getSize().x;
	p2.y += sin(rect.getRotation() * PI / 180) * rect.getSize().x;

	m = (p2.y - p1.y) / (p2.x - p1.x);
	q = p1.y - m * p1.x;

	line[0].position = p1;
	line[1].position = p2;
}

void Block::draw(sf::RenderWindow& window)
{
	window.draw(body);
	//window.draw(line);
}

bool Block::reflects()
{
	return isReflective;
}
