#pragma once
#include <SFML/Graphics.hpp>

class Block
{
public:
	Block();
	void draw(sf::RenderWindow& window);
	void move(sf::Vector2f lastMousePos);
	bool contains(sf::Vector2f p);

private:
	sf::VertexArray body;
};