#include "simulation.h"

Simulation::Simulation()
{
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2.f / 3.f;
	windowSize.y = windowSize.x * 9.f / 16.f;
}

int Simulation::run()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Laser simulation", sf::Style::Default, settings);
	window.setView(sf::View(sf::Vector2f(viewSize.x / 2, viewSize.y / 2), sf::Vector2f(viewSize.x, viewSize.y)));	
	window.setFramerateLimit(60);
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

		window.clear(sf::Color(150, 150, 150));

		update();
		draw();

		window.display();
	}

	return 0;
}

void Simulation::update()
{
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition());

	lastMousePos = mousePos;
}

void Simulation::draw()
{
	laser1.draw(window);
}
