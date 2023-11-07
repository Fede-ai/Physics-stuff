#pragma once
#include <SFML/Graphics.hpp>
#include <stack>
#include "wall.h"

class Laser
{
public:
	Laser(sf::Vector2f pos);
	void move(sf::Vector2f pos, sf::Vector2f lastPos);
	void updateLaser(std::vector<Wall> walls);
	void draw(sf::RenderWindow& window);

private:
	static sf::Vector2f intersection(float m1, float q1, float m2, float q2);
	static bool contains(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p);

	bool wasClickingLeft = false;
	bool isMoving = false;
	bool wasClickingRight = false;
	bool isRotating = false;

	sf::RectangleShape body;
	sf::VertexArray laser;
	bool isEnded = false;
};