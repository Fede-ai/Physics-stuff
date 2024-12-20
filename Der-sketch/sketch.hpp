#pragma once
#include <SFML/Graphics.hpp>

class Sketch {
public:
	Sketch(sf::RenderWindow& inW);
	void update();
	void draw() const;

private:
	sf::RenderWindow& w;
	std::vector<std::vector<sf::Vector2f>> points;
	bool isDrawing = false;
};