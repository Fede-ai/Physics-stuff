#pragma once
#include <SFML/Graphics.hpp>

class Pendulum {
public:
	Pendulum();
	void update();
	void moveGraph();
	sf::Image renderGraph(int width, int height);
	void setPendulum(int x, int y, int width, int height);
	void applyZoom(float delta);

	static constexpr float pi = 3.1415f;

	sf::Vector2<double> pendulum = { 0.25, 2 };
	bool isUpdating = false;
	bool isMovingGraph = false;

private:
	sf::Vector2f anchor = sf::Vector2f(0, 0);
	sf::Vector2f defaultSize = sf::Vector2f(0, 0);

	sf::Vector2i unit = sf::Vector2i(0, 0);
	float zoom = 1;

	std::vector<sf::Vector2<double>> trail;
	sf::Vector2i lastMousePos = sf::Mouse::getPosition();

	double length = 1, drag = 0.3;
};

