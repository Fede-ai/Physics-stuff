#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

class Vec2D {
public:
	Vec2D(float inX, float inY) : x(inX), y(inY) {}
	float dot(Vec2D o) const {
		return x * o.x + y * o.y;
	}
	float mag() const {
		return float(sqrt(x * x + y * y));
	}
	Vec2D operator+(Vec2D o) const {
		return Vec2D(x + o.x, y + o.y);
	}
	Vec2D operator-(Vec2D o) const {
		return Vec2D(x - o.x, y - o.y);
	}
	Vec2D operator*(float k) const {
		return Vec2D(x * k, y * k);
	}
	Vec2D operator/(float k) const {
		return Vec2D(x / k, y / k);
	}

	float x = 0, y = 0;
};

class Ball {
public:
	Ball(sf::Color c, int inM) : m(inM) {
		vr.setSize({ 0, 10 });
		vr.setOrigin({ 0, 5 });

		obj.setRadius(float(sqrt(m)));
		obj.setFillColor(c);
		obj.setOutlineThickness(-5);
		obj.setOrigin({ obj.getRadius(), obj.getRadius() });
		obj.setPosition({ p.x, p.y });
	}

	void draw(sf::RenderWindow& w) {
		vr.setSize({ v.mag() * 20 + obj.getRadius(), vr.getSize().y });
		vr.setPosition({ p.x, p.y });
		vr.setRotation(sf::radians(float(atan2(v.y, v.x))));
		w.draw(vr);

		obj.setPosition({ p.x, p.y });
		w.draw(obj);
	};

	void move() {
		p = p + v;
	}

	void collideWall() {
		if (p.x < obj.getRadius())
			p.x = obj.getRadius(), v.x = -v.x;
		else if (p.x > 1920 - obj.getRadius())
			p.x = 1920 - obj.getRadius(), v.x = -v.x;
		if (p.y < obj.getRadius())
			p.y = obj.getRadius(), v.y = -v.y;
		else if (p.y > 1080 - obj.getRadius())
			p.y = 1080 - obj.getRadius(), v.y = -v.y;
	}

	Vec2D v = Vec2D(0, 0);
	Vec2D p = Vec2D(0, 0);
	int m = 10;
	sf::RectangleShape vr;
	sf::CircleShape obj;
};

static void printAll(int m1, Vec2D v1, int m2, Vec2D v2) {
	std::cout << "p1x = " << v1.x * m1 << " kg*m/s, p1y = " << v1.y * m1 << " kg*m/s\n";
	std::cout << "p2x = " << v2.x * m2 << " kg*m/s, p2y = " << v2.y * m2 << " kg*m/s\n";
	std::cout << "K1 = " << v1.mag() * v1.mag() * m1 / 2 <<
		" J, K2 = " << v2.mag() * v2.mag() * m2 / 2 << " J\n\n";
}

int main() {
	sf::ContextSettings settings;
	settings.antiAliasingLevel = 8;

	float width = sf::VideoMode::getDesktopMode().size.x * 2 / 3.f;
	auto vm = sf::VideoMode({ unsigned int(width), unsigned int(width * 9 / 16.f) });
	sf::RenderWindow w(vm, "Elastic Collision", sf::State::Windowed, settings);
	w.setView(sf::View({ 960, 540 }, { 1920, 1080 }));
	w.setFramerateLimit(100);

	bool playing = false;
	int moving = 0, speeding = 0;

	Ball balls[4] = { { sf::Color::Blue, 500 }, { sf::Color::Red, 2000 },
	{ sf::Color::Yellow, 5000 }, { sf::Color::Green, 1000 } };

	balls[0].p = Vec2D(640, 360), balls[0].v = Vec2D(3, 0);
	balls[1].p = Vec2D(1280, 360), balls[1].v = Vec2D(-2, 2);
	balls[2].p = Vec2D(640, 720), balls[2].v = Vec2D(3, 0);
	balls[3].p = Vec2D(1280, 720), balls[3].v = Vec2D(-2, 2);

	//printAll(m1, v1, m2, v2);

	while (w.isOpen()) {
		while (const std::optional event = w.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				w.close();
			}
			else if (const auto e = event->getIf<sf::Event::KeyPressed>()) {
				if (e->code == sf::Keyboard::Key::Space) {
					playing = !playing;
					moving = 0, speeding = 0;
				}
			}
			else if (const auto e = event->getIf<sf::Event::MouseButtonPressed>()) {
				sf::Vector2f pos = w.mapPixelToCoords(sf::Mouse::getPosition(w));
				Vec2D m(pos.x, pos.y);
				moving = 0, speeding = 0;
				if (e->button == sf::Mouse::Button::Left) {
					for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++) {
						if ((m - balls[i].p).mag() < balls[i].obj.getRadius()) {
							moving = i + 1;
							break;
						}
					}
				}
				else if (e->button == sf::Mouse::Button::Right) {
					for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++) {
						if ((m - balls[i].p).mag() < balls[i].obj.getRadius()) {
							speeding = i + 1;
							break;
						}
					}
				}
			}
			else if (const auto e = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (e->button == sf::Mouse::Button::Left)
					moving = 0;
				if (e->button == sf::Mouse::Button::Right)
					speeding = 0;
			}
		}

		if (playing) {
			for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++)
				balls[i].move();

			for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++) {
				for (int j = 0; j < sizeof(balls) / sizeof(Ball); j++) {
					if (j >= i)
						break;

					if ((balls[i].p - balls[j].p).mag() < balls[i].obj.getRadius() + balls[j].obj.getRadius()) {
						Vec2D nHat = (balls[j].p - balls[i].p) / (balls[j].p - balls[i].p).mag();
						Vec2D tHat = Vec2D(-nHat.y, nHat.x);

						float v1n = balls[i].v.dot(nHat), v2n = balls[j].v.dot(nHat);
						float v1t = balls[i].v.dot(tHat), v2t = balls[j].v.dot(tHat);

						float v1nf = (v1n * (balls[i].m - balls[j].m) + 2 * balls[j].m * v2n) / (balls[i].m + balls[j].m);
						float v2nf = (v2n * (balls[j].m - balls[i].m) + 2 * balls[i].m * v1n) / (balls[i].m + balls[j].m);

						balls[i].v = nHat * v1nf + tHat * v1t;
						balls[j].v = nHat * v2nf + tHat * v2t;
					}
				}
			}

			for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++)
				balls[i].collideWall();
		}
		else {
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				moving = 0;
			if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
				speeding = 0;

			sf::Vector2f pos = w.mapPixelToCoords(sf::Mouse::getPosition(w));
			Vec2D m(pos.x, pos.y);
			if (moving != 0)
				balls[moving - 1].p = m;
			if (speeding != 0)
				balls[speeding - 1].v = (m - balls[speeding - 1].p) / 20;
		}

		w.clear(sf::Color(30, 30, 30));

		for (int i = 0; i < sizeof(balls) / sizeof(Ball); i++)
			balls[i].draw(w);

		w.display();
	}

	return 0;
}