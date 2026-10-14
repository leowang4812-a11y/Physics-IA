#pragma once
#include <SFML/Graphics.hpp>
#include "objectHandler.h"
#include "settings.h"

class Display {
public:
	Display(const ObjectHandler& object_handler)
		: m_window{ sf::VideoMode{ Settings::display_width , Settings::display_height }, "Physics IA Simulation", sf::Style::Default }
		, m_object_handler{ object_handler } {

		cosmeticSpheresInit();
		m_window.setVerticalSyncEnabled(false);
		m_window.setFramerateLimit(0);
	}

	void update() {
		cosmeticSpheresUpdate();
	}

	void pollEvents() {
		sf::Event evnt;
		while (m_window.pollEvent(evnt)) {
			if (evnt.type == evnt.Closed) {
				m_window.close();
			}
		}
	}

	void clear() {
		m_window.clear(sf::Color(0, 0, 0));
	}

	void draw() {
		for (const auto& circle : m_cosmetic_spheres) {
			m_window.draw(circle);
		}
		drawLines();
	}

	void display() {
		m_window.display();
	}

	const sf::RenderWindow& getRenderWindow() const { return m_window; }

private:
	void cosmeticSpheresInit() {
		for (int i = 0; i < ((m_object_handler.getSpheres().size() > 100) ? 100 : m_object_handler.getSpheres().size()); ++i) {
			sf::CircleShape circle(m_object_handler.getSpheres()[i].getRadius());

			circle.setFillColor(sf::Color(255, 255, 255));
			circle.setOrigin(circle.getRadius(), circle.getRadius());

			m_cosmetic_spheres.emplace_back(circle);
		}
	}
	void cosmeticSpheresUpdate() {
		auto& spheres = m_object_handler.getSpheres();
		for (int i = 0; i < m_cosmetic_spheres.size(); ++i) {
			int grayscale = 105 + static_cast<int>(150 * (Settings::container_depth - spheres[i].getPos().z) / Settings::container_depth);

			m_cosmetic_spheres[i].setPosition(spheres[i].getPos().x * Settings::display_width / Settings::container_width, 
				spheres[i].getPos().y * Settings::display_height / Settings::container_height);
			m_cosmetic_spheres[i].setRadius(spheres[i].getRadius() * (Settings::display_width / Settings::container_width) * (1 - 0.7 * (spheres[i].getPos().z / Settings::container_depth)));
			m_cosmetic_spheres[i].setFillColor(sf::Color(grayscale, grayscale, grayscale));
		}
	}
	
	void drawLines() {
		//sf::RectangleShape line(sf::Vector2f{ 10, Settings::display_height });
		//line.setPosition(sf::Vector2f{ Settings::container_top_left.x - line.getSize().x, Settings::container_top_left.y}); 
		//m_window.draw(line);
	}

	sf::RenderWindow m_window;
	const ObjectHandler& m_object_handler;
	std::vector<sf::CircleShape> m_cosmetic_spheres;
};


