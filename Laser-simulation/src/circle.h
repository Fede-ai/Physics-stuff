#pragma once
#include <SFML/Graphics.hpp>

class Circle
{
public:
	Circle(sf::CircleShape circle, char inType);
	void draw(sf::RenderWindow& window);
	bool reflects();

private:
	sf::CircleShape body;
	bool isReflective;
};

