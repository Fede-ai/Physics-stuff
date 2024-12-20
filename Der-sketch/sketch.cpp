#include "sketch.hpp"
#include <iostream>

Sketch::Sketch(sf::RenderWindow& inW)
	:
	w(inW)
{
}

void Sketch::update()
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//start a new line
		if (!isDrawing) {
			points.push_back({});
			isDrawing = true;
		}

		sf::Vector2f p = w.mapPixelToCoords(sf::Mouse::getPosition(w));
		points[points.size() - 1].push_back(p);
	}
	else if (isDrawing) {
		isDrawing = false;
	}
}

void Sketch::draw() const
{
	auto& v = w.getView();
	const sf::Color col = sf::Color(100, 100, 100);
	sf::VertexArray axes(sf::PrimitiveType::Lines);

	const float yTop = v.getCenter().y - v.getSize().y / 2.f;
	const float yBot = v.getCenter().y + v.getSize().y / 2.f;
	const float xDiff = 100;
	//draw vertical axes
	int start = int(std::floor((v.getCenter().x - v.getSize().x / 2.f) / xDiff)) + 1;
	for (int x = 0; x < v.getSize().x / xDiff; x++) {
		if (start + x == 0)
			continue;

		axes.append({ sf::Vector2f((start + x) * xDiff, yTop), col });
		axes.append({ sf::Vector2f((start + x) * xDiff, yBot), col });
	}

	const float xTop = v.getCenter().x - v.getSize().x / 2.f;
	const float xBot = v.getCenter().x + v.getSize().x / 2.f;
	const float yDiff = 100;
	//draw horizontal axes
	start = int(std::floor((v.getCenter().y - v.getSize().y / 2.f) / yDiff)) + 1;
	for (int y = 0; y < v.getSize().y / yDiff; y++) {
		if (start + y == 0)
			continue;

		axes.append({ sf::Vector2f(xTop, (start + y) * yDiff), col });
		axes.append({ sf::Vector2f(xBot, (start + y) * yDiff), col });
	}
	
	//draw capital axes
	axes.append({ sf::Vector2f(xTop, 0) });
	axes.append({ sf::Vector2f(xBot, 0) });
	axes.append({ sf::Vector2f(0, yTop) });
	axes.append({ sf::Vector2f(0, yBot) });

	w.draw(axes);

	for (const auto& line : points) {
		sf::VertexArray fun(sf::PrimitiveType::LineStrip);
		for (const auto& p : line)
			fun.append({ p, sf::Color(40, 240, 40) });
		w.draw(fun);
		
		if (line.size() < 3)
			continue;

		sf::VertexArray der(sf::PrimitiveType::LineStrip);
		for (int i = 0; i < line.size() - 1; i++) {
			const auto &a = line[i], &b = line[i + 1];
			float m = (b.y - a.y) / (b.x - a.x);
			der.append({ sf::Vector2f((b.x + a.x) / 2.f, m * yDiff), sf::Color(240, 40, 40) });
		}
		w.draw(der);
	}
}
