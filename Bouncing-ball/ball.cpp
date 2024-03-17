#include "ball.h"

Ball::Ball()
{
}

int Ball::run()
{
	const auto screenSize = sf::VideoMode::getDesktopMode();

	w.create(sf::VideoMode(screenSize.width * 2/3, screenSize.width * 3 / 8), "Bouncing ball");
	w.setView(sf::View(sf::Vector2f(1920/2, 1080/2), sf::Vector2f(1920, 1080)));
	w.setFramerateLimit(100);

	while (w.isOpen()) {
		sf::Event e;
		while (w.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				w.close();
		}

		w.clear(sf::Color(130, 130, 130));

		w.display();
	}

	return 0;
}
