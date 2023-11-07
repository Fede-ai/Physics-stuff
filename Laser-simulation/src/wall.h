#pragma once
#include <SFML/Graphics.hpp>

class Wall
{
public:
	Wall(sf::RectangleShape rect, char inType);
	void draw(sf::RenderWindow& window);

private:
	sf::RectangleShape body;
	char type;
};