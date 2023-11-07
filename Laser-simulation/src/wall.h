#pragma once
#include <SFML/Graphics.hpp>

class Wall
{
public:
	Wall(sf::RectangleShape rect, char inType);
	void draw(sf::RenderWindow& window);
	bool reflects();	
	float m, q;
	sf::Vector2f p1, p2;

private:
	sf::RectangleShape body;
	bool isReflective;

	sf::VertexArray line = sf::VertexArray(sf::Lines, 2);
};