#pragma once
#include <SFML/Graphics.hpp>

class Pendulum
{
public:
	Pendulum();
	int run();

private:
	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::ContextSettings settings;
};

