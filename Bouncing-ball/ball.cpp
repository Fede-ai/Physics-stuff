#include "ball.h"
#include <chrono>
#include <iostream>

Ball::Ball()
	:
	body(sf::PrimitiveType::TriangleFan)
{
}

int Ball::run()
{
	const auto screenSize = sf::VideoMode::getDesktopMode();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	w.create(sf::VideoMode(screenSize.width * 2/3, screenSize.width * 3 / 8), "Bouncing ball", sf::Style::Default, settings);
	w.setView(sf::View(sf::Vector2f(1920/2, 1080/2), sf::Vector2f(1920, 1080)));
	w.setFramerateLimit(100);

	while (w.isOpen()) {
		sf::Event e;
		while (w.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				w.close();
		}

		w.clear(sf::Color(130, 130, 130));

		update();
		draw();

		w.display();
	}

	return 0;
}

void Ball::update()
{
	float length = sqrt(area / PI * squeezed);
	float height = sqrt(area / PI / squeezed);
	float ang = atan(vel.y / vel.x);

	body.clear();
	body.append(sf::Vertex(pos, sf::Color::Black));
	for (int i = 0; i < sidesBall + 2; i++) {
		float circleAng = i / (sidesBall / 2.f) * PI;
		float x = cos(circleAng) * length, y = sin(circleAng) * height;
		float dist = sqrt(std::pow(x, 2) + std::pow(y, 2));
		float pointAng = atan(y / x);
		if (x < 0)
			pointAng += PI;

		sf::Vertex v;
		v.position.x = pos.x + cos(ang + pointAng) * dist;
		v.position.y = pos.y + sin(ang + pointAng) * dist;

		v.color = sf::Color::Black;
		body.append(v);
	}
}

void Ball::draw()
{
	w.draw(body);
}