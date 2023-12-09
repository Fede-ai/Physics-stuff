#pragma once
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

class Exercise
{
public:
	Exercise();
	int run();

private:
	void setExerciseTime();
	void draw();

	static size_t time();

	sf::RenderWindow window;
	bool isFullscreen = false;
	bool canFullscreen = false;
	size_t lastTime = time();
	size_t exerciseTime = 0;
	bool isPaused = true;
	const double meter = 300;

	sf::VertexArray background;
	sf::VertexArray ground;

	sf::RectangleShape spring;
	sf::CircleShape sphere1;
};