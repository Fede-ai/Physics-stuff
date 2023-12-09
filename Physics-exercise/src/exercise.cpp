#include "exercise.h"
#include <iostream>
constexpr auto PI = 3.14159265358979323846;

Exercise::Exercise()
{
	ground.setPrimitiveType(sf::TrianglesStrip);
	ground.append(sf::Vertex(sf::Vector2f(0, -100)));
	ground.append(sf::Vertex(sf::Vector2f(1200, 700)));
	ground.append(sf::Vertex(sf::Vector2f(1200, 0)));
	ground.append(sf::Vertex(sf::Vector2f(2000, 700)));

	spring.setOrigin(0, 0.1 * meter);
	spring.setRotation(atan(2 / 3.f) * 180 / PI);
	spring.setFillColor(sf::Color(100, 100, 100));
	spring.setPosition(782.5, 458);

	sphere1.setFillColor(sf::Color(200, 200, 200));
	sphere1.setRadius(0.14 * meter);
	sphere1.setOrigin(0.14 * meter, 0.14 * meter);
}

int Exercise::run()
{
	sf::Vector2f windowSize;
	windowSize.x = sf::VideoMode::getDesktopMode().width * 2 / 3;
	windowSize.y = windowSize.x * 9 / 16;

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	window.create(sf::VideoMode(windowSize.x, windowSize.y), "Physics exercise", sf::Style::Default, settings);
	window.setFramerateLimit(60);	
	window.setView(sf::View(sf::Vector2f(1920 / 2, 1080 / 2), sf::Vector2f(1920, -1080)));
	lastTime = time();

	setExerciseTime();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Enter && event.key.alt && canFullscreen)
				{
					sf::View view = window.getView();
					if (!isFullscreen)
						window.create(sf::VideoMode::getDesktopMode(), "Physics exercise", sf::Style::Fullscreen, settings);
					else
						window.create(sf::VideoMode(windowSize.x, windowSize.y), "Physics exercise", sf::Style::Default, settings);
					window.setView(view);
					window.setFramerateLimit(60);
					canFullscreen = false;
					isFullscreen = !isFullscreen;
				}
				else if (event.key.code == sf::Keyboard::Space)
					isPaused = !isPaused;
			}
		}

		if (!isPaused)
			setExerciseTime();

		window.clear();
		draw();
		window.display();		
		
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			canFullscreen = true;		
		
		if (!isPaused)
			exerciseTime += time() - lastTime;
		lastTime = time();
	}

	return 0;
}

void Exercise::setExerciseTime()
{
	double seconds = exerciseTime / 1000.f;

	sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
	std::cout << "x: " << mousePos.x << ", y: " << mousePos.y << "\n";

	spring.setSize(sf::Vector2f(std::min((0.25 + 2 * seconds) * meter, 0.625 * meter), 0.24 * meter));
	sphere1.setPosition(874, 700 + 0.14 * meter - 0.7 * meter);
}

void Exercise::draw()
{
	background.setPrimitiveType(sf::TriangleFan);
	background.clear();
	background.resize(22);
	background[0].position = window.getView().getCenter();
	background[0].color = sf::Color(0, 200, 200);
	int radius = 775;
	for (int i = 0; i < 21; i++)
	{
		float ang = 2 * PI * (float(i) / (20));
		sf::Vector2f pos(cos(ang) * radius * 16 / 9, sin(ang) * radius);
		background[i + 1].position = pos + window.getView().getCenter();
		background[i + 1].color = sf::Color(0, 50, 130);
	}
	window.draw(background);
	window.draw(ground);

	window.draw(spring);
	window.draw(sphere1);
}

size_t Exercise::time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}