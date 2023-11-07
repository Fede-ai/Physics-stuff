#include "simulation.h"
constexpr auto PI = 3.14159265358979323846;

Simulation::Simulation()
{
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2.f / 3.f;
	windowSize.y = windowSize.x * 9.f / 16.f;

	int n = 21;
	background.setPrimitiveType(sf::TriangleFan);
	background.resize(n + 1);
	background[0].position = sf::Vector2f(1920/2, 1080/2);
	background[0].color = sf::Color(0, 200, 200);
	int radius = 775;
	for (int i = 0; i < n; i++)
	{
		float ang = 2 * PI * (float(i) / (n - 1));
		sf::Vector2f pos(cos(ang) * radius * 16 / 9, sin(ang) * radius);
		background[i + 1].position = pos + sf::Vector2f(1920 / 2, 1080 / 2);
		background[i + 1].color = sf::Color(0, 50, 130);
	}
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
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
		buildingType = 'r';
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
		buildingType = 'n';
	if (buildingType != ' ')
	{		
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (isBuilding == false)
			{
				sf::RectangleShape newBuilding;
				newBuilding.setPosition(mousePos);
				building = newBuilding;
				isBuilding = true;
			}
			sf::Vector2f diff(mousePos - building.getPosition());
			building.setSize(sf::Vector2f(sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)), 4));
			float ang = atan(diff.y / diff.x) * 180 / PI;
			if (mousePos.x < building.getPosition().x)
				ang -= 180;
			building.setRotation(ang);
		}
		else if (isBuilding == true)
		{
			isBuilding = false;
			blocks.push_back(Wall(building, buildingType));
			buildingType = ' ';
		}
	}

	lastMousePos = mousePos;
}

void Simulation::draw()
{
	window.draw(background);
	for (auto block : blocks)
	{
		block.draw(window);
	}
	if (isBuilding)
		window.draw(building);
}
