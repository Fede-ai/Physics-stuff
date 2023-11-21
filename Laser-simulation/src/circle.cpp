#include "circle.h"
#include <iostream>

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

	x = circle.getPosition().x;
	y = circle.getPosition().y;
	r = circle.getRadius();
}

void Circle::draw(sf::RenderWindow& window)
{
	window.draw(body);
}

bool Circle::move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos)
{
	bool needUpdate = false;

	if (leftEvent)
		isMoving = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && isMoving)
		isMoving = false;

	if (isMoving)
	{
		needUpdate = true;
		body.move(pos - lastPos);
	}

	if (rightEvent)
		isRotating = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right) && isRotating)
		isRotating = false;

	if (isRotating)
	{
		needUpdate = true;
		sf::Vector2f diff = pos - body.getPosition();
		body.setRadius(sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)));
		body.setOrigin(body.getRadius(), body.getRadius());
	}

	if (needUpdate)
	{
		//p1 = body.getPosition();
		//p2 = body.getPosition();
		//p2.x += cos(body.getRotation() * PI / 180) * body.getSize().x;
		//p2.y += sin(body.getRotation() * PI / 180) * body.getSize().x;
		//
		//m = (p2.y - p1.y) / (p2.x - p1.x);
		//q = p1.y - m * p1.x;
	}

	return needUpdate;
}

bool Circle::reflects()
{
	return isReflective;
}

sf::FloatRect Circle::hitbox()
{
	return body.getGlobalBounds();
}
