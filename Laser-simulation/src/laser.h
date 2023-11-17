#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "block.h"

class Laser
{
public:
	Laser(sf::Vector2f pos);
	bool move(bool leftEvent, bool rightEvent, sf::Vector2f pos, sf::Vector2f lastPos);
	void updateLaser(std::vector<Block> blocks);
	void drawBody(sf::RenderWindow& window);
	void drawLaser(sf::RenderWindow& window);
	sf::FloatRect hitbox();

private:
	std::pair<sf::Vector2f, int> findCollision(std::vector<Block> blocks, sf::Vector2f start, float ang, int exclude);
	float findNewAngle(float angBefore, Block wall);

	bool isMoving = false;
	bool isRotating = false;

	sf::RectangleShape body;
	sf::VertexArray laser;
};