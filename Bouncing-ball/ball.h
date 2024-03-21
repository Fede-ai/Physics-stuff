#pragma once
#include <SFML/Graphics.hpp>

#define PI 3.1415

class Ball
{
public:
	Ball();
	int run();
	void update();
	void draw();

private:
	sf::RenderWindow w;

	sf::VertexArray body;
	sf::Vector2f pos = sf::Vector2f(540, 540);
	sf::Vector2f vel = sf::Vector2f(-1, 1);
	float area = 10'000;
	float squeezed = 1;
	const int sidesBall = 40;
	const float squeezMomentum = 0.7;
	const float squeezCoef = 20;
	float ang = 0;
};

