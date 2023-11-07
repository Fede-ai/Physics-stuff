#include "laser.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Laser::Laser(sf::Vector2f pos)
{
	body.setSize(sf::Vector2f(100, 30));
	body.setOrigin(50, 15);	
	body.setPosition(pos);
	body.setFillColor(sf::Color(30, 30, 30));

	laser.setPrimitiveType(sf::LineStrip);
}

void Laser::move(sf::Vector2f pos, sf::Vector2f lastPos)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !wasClickingLeft && body.getGlobalBounds().contains(pos))
		isMoving = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && isMoving)
		isMoving = false;

	if (isMoving)
		body.move(pos - lastPos);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && !wasClickingRight && body.getGlobalBounds().contains(pos) && !isMoving)
		isRotating = true;
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Right) && isRotating)
		isRotating = false;

	if (isRotating)
	{
		sf::Vector2f diff = pos - body.getPosition();
		float ang = atan(diff.y / diff.x) * 180 / PI;
		if (pos.x < body.getPosition().x)
			ang -= 180;

		if (ang < 0)
			ang += 360;

		//if (ang == 90 || ang == 270)
		//	ang += 1;

		body.setRotation(ang);
	}

	wasClickingLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left);
	wasClickingRight = sf::Mouse::isButtonPressed(sf::Mouse::Right);
}

void Laser::updateLaser(std::vector<Wall> walls)
{
	laser.clear();
	laser.append(body.getPosition());
	float m = tan(body.getRotation() * PI / 180); 
	float q = body.getPosition().y - m * body.getPosition().x;

	for (auto wall : walls)
	{
		//std::cout << body.getRotation() << "\n";
		sf::Vector2f i = intersection(m, q, wall.m, wall.q);
		if (contains(wall.p1, wall.p2, i))
		{
			if (((body.getRotation() < 90 || body.getRotation() > 270) && i.x > body.getPosition().x) ||
				(body.getRotation() > 90 && body.getRotation() < 270 && i.x < body.getPosition().x))
			{
				laser.append(i);
				break;
			}
		}
	}


	//while (!isEnded)
	//{
	//
	//}
}

void Laser::draw(sf::RenderWindow& window)
{
	window.draw(laser);
	window.draw(body);
}

sf::Vector2f Laser::intersection(float m1, float q1, float m2, float q2)
{
	float x = (q2 - q1) / (m1 - m2);
	float y = m1 * x + q1;

	return sf::Vector2f(x, y);
}

bool Laser::contains(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p)
{
	float minX = std::min(p1.x, p2.x);
	float maxX = std::max(p1.x, p2.x);
	float minY = std::min(p1.y, p2.y);
	float maxY = std::max(p1.y, p2.y);

	//check the point is within the bounding box of the segment
	if (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY)
		return true;

	return false;
}
