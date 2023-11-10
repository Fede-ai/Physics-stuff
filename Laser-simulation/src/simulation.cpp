#include "simulation.h"
#include <iostream>
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
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
					clickEvent = true;
			}
		}

		window.clear(sf::Color(150, 150, 150));

		move();
		draw();

		window.display();
	}

	return 0;
}

void Simulation::move()
{
	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		if (canSelectBuilding)
			buildingType = 'r';
		canSelectBuilding = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		if (canSelectBuilding)
			buildingType = 'n';
		canSelectBuilding = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		if (canSelectBuilding)
			buildingType = 'l';
		canSelectBuilding = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
		if (canSelectBuilding)
			buildingType = 's';
		canSelectBuilding = false;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
		if (canSelectBuilding)
			buildingType = 'f';
		canSelectBuilding = false;
	}
	else
	{
		canSelectBuilding = true;
	}

	//create new walls
	if (buildingType == 'r' || buildingType == 'n')
	{		
		if (isBuildingCircle == false)
		{
			//create-modify in-construction wall
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				//create a new wall
				if (isBuildingWall == false)
				{
					clickEvent = false;
					sf::RectangleShape newBuilding;
					newBuilding.setOrigin(0, wallThickness / 2);
					newBuilding.setPosition(mousePos);
					wall = newBuilding;
					isBuildingWall = true;
				}
				//tilt and stretch the wall 
				sf::Vector2f diff(mousePos - wall.getPosition());
				wall.setSize(sf::Vector2f(sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)), wallThickness));
				float ang = atan(diff.y / diff.x) * 180 / PI;
				if (mousePos.x < wall.getPosition().x)
					ang -= 180;
				wall.setRotation(ang);
			}
			//conferm wall and stop building
			else if (isBuildingWall == true)
			{
				updateNeeded = true;
				isBuildingWall = false;
				walls.push_back(Block(wall, buildingType));
				buildingType = ' ';
			}
		}
	}
	//create new walls
	if (buildingType == 's' || buildingType == 'f')
	{
		if (isBuildingWall == false)
		{
			//create-modify in-construction circle
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				//create a new circle
				if (isBuildingCircle == false)
				{
					clickEvent = false;
					sf::CircleShape newBuilding;
					newBuilding.setPosition(mousePos);
					circle = newBuilding;
					isBuildingCircle = true;
				}
				//tilt and stretch the circle 
				sf::Vector2f diff(mousePos - circle.getPosition());
				circle.setRadius(sqrt(std::pow(diff.x, 2) + std::pow(diff.y, 2)));
				circle.setOrigin(circle.getRadius(), circle.getRadius());
			}
			//conferm circle and stop building
			else if (isBuildingCircle == true)
			{
				updateNeeded = true;
				isBuildingCircle = false;
				//walls.push_back(Block(wall, buildingType));
				buildingType = ' ';
			}
		}
	}
	//create new lasers
	if (buildingType == 'l')
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			updateNeeded = true;
			clickEvent = false;
			lasers.push_back(Laser(mousePos));
			buildingType = ' ';
		}
	}

	for (auto& laser : lasers)
	{
		if (laser.hitbox().contains(mousePos) && clickEvent)
		{
			if (laser.move(true, mousePos, lastMousePos))
				updateNeeded = true;
			clickEvent = false;
		}
		else 
		{
			if (laser.move(false, mousePos, lastMousePos))
				updateNeeded = true;
		}
	}

	if (updateNeeded)
	{
		for (auto& laser : lasers)
			laser.updateLaser(walls);
		updateNeeded = false;
	}

	clickEvent = false;
	lastMousePos = mousePos;
}

void Simulation::draw()
{
	window.draw(background);

	for (auto wall : walls)
		wall.draw(window);

	if (isBuildingWall)
		window.draw(wall);

	if (isBuildingCircle)
		window.draw(circle);

	for (auto laser : lasers)
		laser.draw(window);
}
