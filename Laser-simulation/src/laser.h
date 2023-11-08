#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "wall.h"

class Laser
{
public:
	Laser(sf::Vector2f pos);
	bool move(bool clicked, sf::Vector2f pos, sf::Vector2f lastPos);
	void updateLaser(std::vector<Wall> walls);
	void draw(sf::RenderWindow& window);
	sf::FloatRect hitbox();

private:
	std::pair<sf::Vector2f, int> findCollision(std::vector<Wall> walls, sf::Vector2f start, float ang, int exclude);
	float findNewAngle(float angBefore, Wall wall);

	bool isMoving = false;
	bool wasClickingRight = false;
	bool isRotating = false;

	sf::RectangleShape body;
	sf::VertexArray laser;
};