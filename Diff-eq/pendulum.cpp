#include "pendulum.hpp"
#include <iostream>

Pendulum::Pendulum()
{
	const float width = sf::VideoMode::getFullscreenModes()[0].width * 2 / 3.f;
	defaultSize.y = width * 9 / 16.f;
	defaultSize.x = width - defaultSize.y * 0.8f;

	unit.x = int(std::round(defaultSize.x / 4.5f));
	unit.y = int(std::round(defaultSize.y / 9.f));
}

void Pendulum::update()
{
	double dt = 0.00001;

	for (int i = 0; i < 1000; i++) {
		double ang = pendulum.x * pi;
		pendulum.x += pendulum.y * dt;
		pendulum.y -= dt * (pendulum.y * drag + 9.806 * sin(ang) / length);
	}

	trail.push_back(pendulum);
}

void Pendulum::moveGraph()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && isMovingGraph) {
		anchor.x += (lastMousePos.x - mousePos.x) / zoom;
		anchor.y += (lastMousePos.y - mousePos.y) / zoom;
	}

	lastMousePos = mousePos;
}

sf::Image Pendulum::renderGraph(int width, int height)
{
	sf::Image img;
	img.create(width, height, sf::Color(50, 50, 50));

	int left = int(anchor.x * zoom - width / 2.f);
	int top = int(anchor.y * zoom - height / 2.f);

	//draw a pixel on the image
	auto draw = [this, &img, width, height](int x, int y, sf::Color c) {
		int xPos = int(x - anchor.x * zoom + img.getSize().x / 2.f);
		int yPos = int(y - anchor.y * zoom + img.getSize().y / 2.f);
		if (xPos >= 0 && xPos < width && yPos >= 0 && yPos < height)
			img.setPixel(xPos, yPos, c);
	};

	//draw vertical axes
	for (int y = top; y < top + height; y++) {
		float xDiff = unit.x * zoom / 2.f;
		for (float x = int(left / xDiff) * xDiff; x < left + width; x += xDiff)
			draw(int(std::round(x)), y, sf::Color(100, 100, 100));

		draw(0, y, sf::Color::Black);
		draw(1, y, sf::Color::Black);
	}

	//draw horizontal axes
	for (int x = left; x < left + width; x++) {
		if (x == 0 || x == 1)
			continue;

		float yDiff = unit.y * zoom;
		for (float y = int(top / yDiff) * yDiff; y < top + height; y += yDiff)
			draw(x, int(std::round(y)), sf::Color(100, 100, 100));

		draw(x, 0, sf::Color::Black);
		draw(x, 1, sf::Color::Black);
	}

	//draw the trail
	if (trail.size() > 1) {
		for (int i = 0; i < trail.size() - 1; i++) {
			double x1 = trail[i].x * unit.x * zoom, x2 = trail[i + 1].x * unit.x * zoom;
			double y1 = -trail[i].y * unit.y * zoom, y2 = -trail[i + 1].y * unit.y * zoom;

			//swap point 1 and 2
			if (y1 > y2) {
				std::swap(y1, y2);
				std::swap(x1, x2);
			}

			for (int y = 0; y < y2 - y1; y++) {
				float per = y / (y2 - y1);
				float x = x2 * per + x1 * (1 - per);


				draw(int(std::round(x)), int(y1 + y), sf::Color::White);
			}
		}
	}

	//draw all the vector arrows
	float yDiff = unit.y * zoom / 4.f;
	for (float y = int(top / yDiff) * yDiff; y < top + height; y += yDiff) {
		
		float xDiff = unit.x * zoom / 8.f;
		for (float x = int(left / xDiff) * xDiff; x < left + width; x += xDiff) {
			
			float xV = x / (unit.x * zoom) * Pendulum::pi;
			float xSize = -y / (unit.y * zoom);
			float ySize = xSize * drag + 9.806 * sin(xV) / length;

			float size = sqrt(xSize * xSize + ySize * ySize);
			float per = std::min(size / 15.f, 1.f);
			auto c = sf::Color(255 * (1 - per), 255 * per, 0);

			for (int i = 0; i < 40; i++) {
				float a = x + i / 40.f * xSize * zoom;
				float b = y + i / 40.f * ySize * zoom;
				draw(int(std::round(a)), int(std::round(b)), c);
			}
		}
	}

	//draw the ball
	int radius = 8;
	int xPen = int(std::round(pendulum.x * unit.x * zoom));
	int yPen = -int(std::round(pendulum.y * unit.y * zoom));
	for (int x = -radius; x < radius + 1; x++) {
		for (int y = -radius; y < radius + 1; y++) {
			if (sqrt(x * x + y * y) < radius)
				draw(xPen + x, yPen + y, sf::Color(200, 10, 10));
		}
	}

	return img;
}

void Pendulum::setPendulum(int x, int y, int width, int height)
{
	int left = int(anchor.x * zoom - width / 2.f);
	int top = int(anchor.y * zoom - height / 2.f);

	pendulum.x = float(x + left) / (unit.x * zoom);
	pendulum.y = -float(y + top) / (unit.y * zoom);

	trail.clear();
	trail.push_back(pendulum);
}

void Pendulum::applyZoom(float delta)
{
	zoom /= (1.f - delta / 10.f);
	zoom = std::max(zoom, 0.4f);
}
