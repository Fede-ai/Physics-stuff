#include "laser.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Laser::Laser(sf::Vector2f pos)
{
	body.setSize(sf::Vector2f(60, 20));
	body.setOrigin(30, 10);	
	body.setPosition(pos);
	body.setFillColor(sf::Color(30, 30, 30));

	laser.setPrimitiveType(sf::LineStrip);
}

bool Laser::move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos)
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
		float ang = atan(diff.y / diff.x) * 180 / PI;
		if (pos.x < body.getPosition().x)
			ang -= 180;

		if (ang < 0)
			ang += 360;

		body.setRotation(ang);
	}

	return needUpdate;
}

void Laser::updateLaser(std::vector<Block> blocks)
{
	auto formatAng = [](float ang) {
		while (ang < 0)
			ang += 360;

		while (ang >= 360)
			ang -= 360;

		return ang;
	};

	int nBounces = 0;
	laser.clear();
	laser.append(body.getPosition());
	float ang = body.getRotation();
	int lastBounceWall = -1;

	while (true)
	{
		const auto info = findCollision(blocks, laser[laser.getVertexCount()-1].position, ang, lastBounceWall);

		laser.append(info.first);

		lastBounceWall = info.second;
		if (lastBounceWall == -1)
			break;

		nBounces++;
		ang = formatAng(findNewAngle(ang, blocks[info.second]));

		if (nBounces >= 200'000)
		{
			nBounces = -1;
			break;
		}
	}

	std::cout << "bounces: " << nBounces << "\n";
}

void Laser::drawBody(sf::RenderWindow& window)
{
	window.draw(body);
}

void Laser::drawLaser(sf::RenderWindow& window)
{
	window.draw(laser);
}

sf::FloatRect Laser::hitbox()
{
	return body.getGlobalBounds();
}

std::pair<sf::Vector2f, int> Laser::findCollision(std::vector<Block> blocks, sf::Vector2f start, float ang, int exclude)
{
	auto intersection = [](float m1, float q1, float m2, float q2) {
		float x = (q2 - q1) / (m1 - m2);
		float y = m2 * x + q2;

		return sf::Vector2f(x, y);
	};
	auto contains = [](sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p) {
		float minX = std::min(p1.x, p2.x);
		float maxX = std::max(p1.x, p2.x);
		float minY = std::min(p1.y, p2.y);
		float maxY = std::max(p1.y, p2.y);

		//check the point is within the bounding box of the segment
		if ((p.x >= minX && p.x <= maxX) || (p.y >= minY && p.y <= maxY))
			return true;

		return false;
	};
	auto dist = [](sf::Vector2f p1, sf::Vector2f p2) {
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		return sqrt(dx * dx + dy * dy);
	};

	//-1 means that the laser will not reflect,
	//otherwise return the index of the wall with wich the laser collides
	int nWall = -1;
	sf::Vector2f closest;
	bool collided = false;
	
	float mLaser = tan(ang * PI / 180);
	float qLaser = - mLaser * start.x + start.y;
	
	for (int i = 0; i < blocks.size(); i++)
	{
		if (i == exclude)
			continue;

		sf::Vector2f inter = intersection(mLaser, qLaser, blocks[i].m, blocks[i].q);
		if (((ang < 90 || ang > 270) && inter.x > start.x) ||
			(ang > 90 && ang < 270 && inter.x < start.x) ||
			(ang == 270 && inter.y < start.y) || (ang == 90 && inter.y > start.y))
		{
			if (contains(blocks[i].p1, blocks[i].p2, inter))
			{
				if (collided == false)
				{
					closest = inter;
					collided = true;
					if (blocks[i].reflects())
						nWall = i;
					else
						nWall = -1;
				}
				else if (dist(start, inter) < dist(start, closest))
				{
					closest = inter;
					if (blocks[i].reflects())
						nWall = i;
					else
						nWall = -1;
				}
			}
		}
	}

	if (!collided)
	{
		if (ang == 0 || ang == 360)
			closest = sf::Vector2f(1920, start.y);
		else if (ang == 180)
			closest = sf::Vector2f(0, start.y);
		else if (ang < 180)
			closest = sf::Vector2f((1080 - qLaser) / mLaser, 1080);
		else if (ang > 180)
			closest = sf::Vector2f(-qLaser / mLaser, 0);
	}

	return std::make_pair(closest, nWall);
}

float Laser::findNewAngle(float angBefore, Block wall)
{
	float wallAng = atan(wall.m) * 180 / PI;
	if (wallAng < 0)
		wallAng = 180 + wallAng;

	return wallAng * 2 - angBefore;
}
