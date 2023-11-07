#include "block.h"

Block::Block()
{
	body.setPrimitiveType(sf::TriangleStrip);
	body.resize(4);

	body[0].position = sf::Vector2f(100, 100);
	body[1].position = sf::Vector2f(300, 100);
	body[2].position = sf::Vector2f(100, 200);
	body[3].position = sf::Vector2f(300, 200);
}

void Block::draw(sf::RenderWindow& window)
{
	window.draw(body);
}

void Block::move(sf::Vector2f lastMousePos)
{
}

bool Block::contains(sf::Vector2f p)
{
	//bool left1 = (p.x - rectCorner1.x) * (rectCorner2.y - rectCorner1.y) - (p.y - rectCorner1.y) * (rectCorner2.x - rectCorner1.x) > 0;
	//bool left2 = (p.x - rectCorner2.x) * (rectCorner3.y - rectCorner2.y) - (p.y - rectCorner2.y) * (rectCorner3.x - rectCorner2.x) > 0;
	//bool left3 = (p.x - rectCorner3.x) * (rectCorner4.y - rectCorner3.y) - (p.y - rectCorner3.y) * (rectCorner4.x - rectCorner3.x) > 0;
	//bool left4 = (p.x - rectCorner4.x) * (rectCorner1.y - rectCorner4.y) - (p.y - rectCorner4.y) * (rectCorner1.x - rectCorner4.x) > 0;

	//return left1 == left2 && left2 == left3 && left3 == left4;
}
