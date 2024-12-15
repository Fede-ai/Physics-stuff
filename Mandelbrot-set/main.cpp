#include <iostream>
#include "SFML/Graphics.hpp"

bool isFinite(double r, double i);

int main() {
	sf::Vector2i imgSize(200, 200);
	sf::Vector2<double> viewSize(2, 2);
	if (imgSize.x / double(imgSize.y) != viewSize.x / viewSize.y)
		return 1;

	sf::Image img;
	img.create(imgSize.x, imgSize.y, sf::Color::Black);

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y / 2.f; y++) {
			double r = (x - imgSize.x / 2.f) * viewSize.x / imgSize.x - 0.5;
			double i = (y - imgSize.y / 2.f) * viewSize.y / imgSize.y;

			sf::Color c;
			if (isFinite(r, i))
				c = sf::Color::White;
			else
				c = sf::Color::Black;

			img.setPixel(x, y, c);
			img.setPixel(x, imgSize.y - y - 1, c);
		}
		std::cout << x + 1 << "/" << imgSize.x << " columns\n";
	}

	img.saveToFile("image.png");

	return 0; 
}

bool isFinite(const double r, const double i) {
	std::vector<std::pair<double, double>> found;
	found.push_back(std::pair<double, double>(0, 0));
	found.push_back(std::pair<double, double>(r, i));

	double newR = r, newI = i;
	for (int a = 0; a < 10000; a++) {
		double tempR = newR * newR - newI * newI + r;
		double tempI = 2 * newR * newI + i;
		newR = tempR, newI = tempI;

		for (const auto& f : found) {
			if (f.first == newR && f.second == newI)
				return true;
		}
		found.push_back(std::pair<double, double>(newR, newI));
	}

	return false;
}