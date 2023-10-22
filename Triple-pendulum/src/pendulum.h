#pragma once
#include <SFML/Graphics.hpp>

class Pendulum
{
public:
	Pendulum();
	int run();

private:
	void update();
	void draw();

	sf::RenderWindow window;
	sf::Vector2i windowSize;
	sf::ContextSettings settings;

	sf::VertexArray background;

	sf::CircleShape firstStart;
	sf::RectangleShape firstBody;
	sf::CircleShape firstEnd;
	sf::Vector2i firstSize = sf::Vector2i(250, 50);
	double firstAng = 0;

	sf::CircleShape secondStart;
	sf::RectangleShape secondBody;
	sf::CircleShape secondEnd;
	sf::Vector2i secondSize = sf::Vector2i(180, 40);
	double secondAng = 0;
	double actualSecondAng = 0;

	sf::CircleShape thirdStart;
	sf::RectangleShape thirdBody;
	sf::CircleShape thirdEnd;
	sf::Vector2i thirdSize = sf::Vector2i(100, 30);
	double thirdAng = 0;
	double actualThirdAng = 0;
};

