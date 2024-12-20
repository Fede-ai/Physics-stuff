#include "sketch.hpp"
#include <iostream>

int main() {
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	const int width = int(sf::VideoMode::getDesktopMode().width * 2 / 3.f);
	const int height = int(sf::VideoMode::getDesktopMode().height * 2 / 3.f);
	const std::string name = "Derivative sketcher";
	bool isFullscreen = false;
	bool isMoving = false;
	const int fpsCap = 10;

	sf::Vector2u lastWinSize = sf::Vector2u(width, height);
	sf::Vector2i lastMousePos = sf::Mouse::getPosition();

	sf::RenderWindow w(sf::VideoMode(width, height), name, sf::Style::Default, settings);
	w.setView(sf::View(sf::Vector2f(0, 0), sf::Vector2f(1920, 1920.f / width * height)));
	w.setFramerateLimit(fpsCap);
	w.setKeyRepeatEnabled(false);

	Sketch sketch(w);

	while (w.isOpen()) {
		sf::Event e;
		while (w.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				w.close();
			//zoom
			else if (e.type == sf::Event::MouseWheelScrolled) {
				float per = 1.f - e.mouseWheelScroll.delta / 10.f;
				auto& v = w.getView();
				w.setView(sf::View(v.getCenter(), { v.getSize().x * per, v.getSize().y * per }));
			}
			//toggle fullscreen
			else if (e.type == sf::Event::KeyPressed) {
				if (e.key.code == sf::Keyboard::Enter && e.key.alt) {
					sf::View v = w.getView();
					if (isFullscreen)
						w.create(sf::VideoMode(width, height), name, sf::Style::Default, settings);
					else
						w.create(sf::VideoMode::getFullscreenModes()[0], name, sf::Style::Fullscreen, settings);
					w.setView(v);
					w.setFramerateLimit(fpsCap);
					w.setKeyRepeatEnabled(false);
					isFullscreen = !isFullscreen;
				}
			}
			//toggle moving
			else if (e.type == sf::Event::MouseButtonPressed) {
				if (e.mouseButton.button == sf::Mouse::Right)
					isMoving = true;
			}
		}

		//scale graph when window changes
		sf::Vector2u winSize = w.getSize();
		if (winSize != lastWinSize) {
			sf::View v = w.getView();
			float xSize = v.getSize().x * winSize.x / float(lastWinSize.x);
			float ySize = v.getSize().y * winSize.y / float(lastWinSize.y);
			w.setView(sf::View(v.getCenter(), {xSize, ySize}));

			lastWinSize = winSize;
		}

		//move graph around
		sf::Vector2i mousePos = sf::Mouse::getPosition();
		if (isMoving) {
			if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
				sf::View v = w.getView();
				v.move((lastMousePos - mousePos).x * v.getSize().x / w.getSize().x,
					(lastMousePos - mousePos).y * v.getSize().y / w.getSize().y);
				w.setView(v);
			}
			else
				isMoving = false;
		}
		lastMousePos = mousePos;

		sketch.update();

		w.clear(sf::Color(20, 20, 20));
		sketch.draw();
		w.display();
	}

	return 0;
}