#include "pendulum.h"

Pendulum::Pendulum()
{
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2 / 3;
	windowSize.y = windowSize.x * 9 / 16;

	settings.antialiasingLevel = 8;
}

int Pendulum::run()
{
	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Pendulum", sf::Style::Default, settings);
	window.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		window.clear(sf::Color(50, 50, 50));

		window.display();
	}

	return 0;
}
