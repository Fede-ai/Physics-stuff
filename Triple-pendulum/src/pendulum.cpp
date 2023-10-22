#include "pendulum.h"
#include <iostream>

Pendulum::Pendulum()
{
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2 / 3;
	windowSize.y = windowSize.x * 9 / 16;

	settings.antialiasingLevel = 4;

	int n = 21;
	background.setPrimitiveType(sf::PrimitiveType::TriangleFan);
	background.resize(n + 1);
	background[0].position = sf::Vector2f(0, 0);
	background[0].color = sf::Color(0, 120, 120);
	int radius = 775;
	for (int i = 0; i < n; i++)
	{
		float ang = 2 * 3.1415 * (float(i) / (n-1));
		sf::Vector2f pos(cos(ang)*radius * 16 / 9, sin(ang)*radius);
		background[i + 1].position = pos;
		background[i + 1].color = sf::Color(0, 50, 130);
	}

	firstStart.setRadius(firstSize.y / 2);
	firstStart.setOrigin(firstSize.y / 2, firstSize.y / 2);
	firstBody.setSize(sf::Vector2f(firstSize));
	firstBody.setOrigin(0, firstSize.y / 2);
	firstEnd = firstStart;

	secondStart.setRadius(secondSize.y / 2);
	secondStart.setOrigin(secondSize.y / 2, secondSize.y / 2);
	secondStart.setFillColor(sf::Color::Black);
	secondBody.setSize(sf::Vector2f(secondSize));
	secondBody.setOrigin(0, secondSize.y / 2);
	secondBody.setFillColor(sf::Color::Black);
	secondEnd = secondStart;

	thirdStart.setRadius(thirdSize.y / 2);
	thirdStart.setOrigin(thirdSize.y / 2, thirdSize.y / 2);
	thirdStart.setFillColor(sf::Color::Red);
	thirdBody.setSize(sf::Vector2f(thirdSize));
	thirdBody.setOrigin(0, thirdSize.y / 2);
	thirdBody.setFillColor(sf::Color::Red);
	thirdEnd = thirdStart;
}

int Pendulum::run()
{
	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Pendulum", sf::Style::Default, settings);
	window.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));
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

		window.clear();
		
		update();
		draw();

		window.display();
	}

	return 0;
}

void Pendulum::update()
{
	firstAng += 0.01;
	firstBody.setRotation(firstAng * 180 / 3.1415);
	firstEnd.setPosition(cos(firstAng) * firstSize.x, sin(firstAng) * firstSize.x);

	secondAng += 0.01;
	actualSecondAng = firstAng + secondAng;
	secondStart.setPosition(firstEnd.getPosition());
	secondBody.setPosition(firstEnd.getPosition());
	secondBody.setRotation(actualSecondAng * 180 / 3.1415);
	sf::Vector2f secondLenght(cos(actualSecondAng) * secondSize.x, sin(actualSecondAng) * secondSize.x);
	secondEnd.setPosition(firstEnd.getPosition() + secondLenght);

	thirdAng += 0.01;
	actualThirdAng = actualSecondAng + thirdAng;
	thirdStart.setPosition(secondEnd.getPosition());
	thirdBody.setPosition(secondEnd.getPosition());
	thirdBody.setRotation(actualThirdAng * 180 / 3.1415);
	sf::Vector2f thirdLenght(cos(actualThirdAng) * thirdSize.x, sin(actualThirdAng) * thirdSize.x);
	thirdEnd.setPosition(secondEnd.getPosition() + thirdLenght);
}

void Pendulum::draw()
{
	window.draw(background);

	window.draw(firstStart);
	window.draw(firstBody);
	window.draw(firstEnd);

	window.draw(secondStart);
	window.draw(secondBody);
	window.draw(secondEnd);

	window.draw(thirdStart);
	window.draw(thirdBody);
	window.draw(thirdEnd);
}
