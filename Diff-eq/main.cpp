#include <iostream>
#include "pendulum.hpp"

int main() {

	Pendulum pendulum;

	const float width = sf::VideoMode::getFullscreenModes()[0].width * 2 / 3.f;
	sf::Vector2f wSize(width, width * 9.f / 16.f);

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;

	sf::RenderWindow w(sf::VideoMode(int(wSize.x), int(wSize.y)), "Pendulum", sf::Style::Default, settings);
	w.setView(sf::View({ wSize.x / 2.f, wSize.y / 2.f }, { wSize.x, wSize.y }));
	w.setKeyRepeatEnabled(false);
	w.setFramerateLimit(50);

	sf::RectangleShape body;
	body.setOrigin(0, 10);
	body.setFillColor(sf::Color(50, 50, 50));

	sf::CircleShape pivot(6);
	pivot.setOrigin(6, 6);
	pivot.setOutlineColor(sf::Color(50, 50, 50));
	pivot.setOutlineThickness(4);
	pivot.setFillColor(sf::Color::Black);

	sf::CircleShape mass(15);
	mass.setOrigin(15, 15);
	mass.setFillColor(sf::Color(200, 10, 10));

	sf::VertexArray arrow(sf::TriangleFan, 9);

	sf::Font font;
	font.loadFromFile("./font.ttf");

	while (w.isOpen()) {
		sf::Event e;
		while (w.pollEvent(e)) {
			if (e.type == sf::Event::Closed)
				w.close();
			else if (e.type == sf::Event::MouseWheelScrolled && e.mouseWheelScroll.x > w.getSize().y * 0.8)
				pendulum.applyZoom(e.mouseWheelScroll.delta);
			else if (e.type == sf::Event::Resized) {
				wSize = sf::Vector2f(w.getSize());
				w.setView(sf::View({ wSize.x / 2.f, wSize.y / 2.f }, { wSize.x, wSize.y }));
			}
			//toogle fullscreen
			else if (e.type == sf::Event::KeyPressed) {
				if (e.key.code == sf::Keyboard::Space)
					pendulum.isUpdating = !pendulum.isUpdating;
				else if (e.key.code == sf::Keyboard::Enter && e.key.alt) {
					if (w.getSize().x != sf::VideoMode::getFullscreenModes()[0].width)
						w.create(sf::VideoMode::getFullscreenModes()[0], "Pendulum", sf::Style::Fullscreen, settings);
					else
						w.create(sf::VideoMode(int(wSize.x), int(wSize.y)), "Pendulum", sf::Style::Default, settings);

					sf::Vector2f newSize = sf::Vector2f(w.getSize());
					w.setView(sf::View({ newSize.x / 2.f, newSize.y / 2.f }, { newSize.x, newSize.y }));
					w.setKeyRepeatEnabled(false);
					w.setFramerateLimit(50);
				}
			}
			//start moving graph
			else if (e.type == sf::Event::MouseButtonPressed) {
				if (e.mouseButton.x > w.getSize().y * 0.8) {
					if (e.mouseButton.button == sf::Mouse::Left)
						pendulum.isMovingGraph = true;
					else if (e.mouseButton.button == sf::Mouse::Right)
						pendulum.setPendulum(e.mouseButton.x - int(w.getSize().y * 0.8), e.mouseButton.y, 
							int(w.getSize().x - w.getSize().y * 0.8), w.getSize().y);
				}
			}
			//stop moving graph
			else if (e.type == sf::Event::MouseButtonReleased) {
				if (e.mouseButton.button == sf::Mouse::Left)
					pendulum.isMovingGraph = false;
			}
		}

		if (pendulum.isUpdating)
			pendulum.update();

		w.clear(sf::Color(20, 20, 20));

		//render graph if needed
		int graphWidth = int(w.getSize().x - w.getSize().y * 0.8);
		if (graphWidth > 0) {
			pendulum.moveGraph();

			sf::Texture texture;
			texture.loadFromImage(pendulum.renderGraph(graphWidth, w.getSize().y));

			sf::Sprite graph(texture);
			graph.setPosition(float(w.getSize().x - graphWidth), 0);
			w.draw(graph);
		}

		float xPos = float(w.getSize().x - std::max(graphWidth, 0)) / 2.f;
		float ang = float(0.5 - pendulum.pendulum.x);

		body.setSize(sf::Vector2f(xPos - 50, 20));
		body.setPosition(xPos, w.getSize().y / 2.f);
		body.setRotation(float(ang * 180));
		w.draw(body);
		
		pivot.setPosition(xPos, w.getSize().y / 2.f);
		w.draw(pivot);

		mass.setPosition(pivot.getPosition());
		mass.move(body.getSize().x * cos(ang * Pendulum::pi), 
			body.getSize().x * sin(ang * Pendulum::pi));
		w.draw(mass);

		float arrowLeng = float(pendulum.pendulum.y * 50);
		sf::Vector2f offset = sf::Vector2f(4 * cos(ang * Pendulum::pi), 4 * sin(ang * Pendulum::pi));
		for (int i = 0; i < 9; i++)
			arrow[i].color = sf::Color(150, 150, 150);

		arrow[0].position = mass.getPosition() + sf::Vector2f(arrowLeng * cos((ang - 0.5f)
			* Pendulum::pi), arrowLeng * sin((ang - 0.5f) * Pendulum::pi));
		arrow[1].position = mass.getPosition() + sf::Vector2f(1.2f * arrowLeng * cos((ang - 0.5f) 
			* Pendulum::pi), 1.2f * arrowLeng * sin((ang - 0.5f) * Pendulum::pi));
		arrow[2].position = arrow[0].position + (offset + offset);
		arrow[3].position = arrow[0].position + offset;
		arrow[4].position = mass.getPosition() + offset;
		arrow[5].position = mass.getPosition() - offset;
		arrow[6].position = arrow[0].position - offset;
		arrow[7].position = arrow[0].position - (offset + offset);
		arrow[8].position = arrow[1].position;
		w.draw(arrow);

		sf::Text text("", font, 25);
		text.setPosition(15, 8);
		text.setString("Ang = " + std::to_string(pendulum.pendulum.x * Pendulum::pi) + " rad\nAng vel = "
			+ std::to_string(pendulum.pendulum.y) + " rad / sec");
		w.draw(text);

		text.setPosition(15, float(w.getSize().y - 45));
		text.setString("A''(t) = -u * A'(t) - (g / L) * sin(A(t))");
		w.draw(text);

		w.display();
	}

	return 0;
}