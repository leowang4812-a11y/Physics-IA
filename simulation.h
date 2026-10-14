#pragma once
#include <SFML\Graphics.hpp>
#include "objectHandler.h"
#include "display.h"
#include <iostream>

class Simulation {
public:
	Simulation()
		: m_object_handler{ static_cast<int>(Settings::num_grains) }
		, m_display{ m_object_handler }
		, m_delta_time{ 1.0 }
		, m_time_elapsed{ 0.0 }
		, m_key_pressed{ false }	
		, m_virial{ 0.0 } 
		, m_accumulated_virial{ 0.0 } 
		, m_temp_uncertainty_max{ 0.0 } {
	}

	double mainLoop(double temperature) {
		sf::Clock clock;
		m_delta_time = Settings::delta_time;
		double actual_delta_time = 0.0;
		double actual_time_elapsed = 0.0;
		while (m_display.getRenderWindow().isOpen()) {
			m_time_elapsed += m_delta_time;
			actual_delta_time = clock.restart().asSeconds();
			actual_time_elapsed += actual_delta_time;
			std::cout << actual_delta_time;
			m_display.pollEvents();
			m_display.clear();
			update();
			displaySimulationCalculations();
			m_display.draw();
			m_display.display();
			//getUserInput();
			m_accumulated_virial += m_virial * m_delta_time;
			m_virial = 0.0;

			if (std::abs(temperature - m_object_handler.getTemp()) / temperature > m_temp_uncertainty_max) {
				m_temp_uncertainty_max = std::abs(temperature - m_object_handler.getTemp()) / temperature;
			}

			if (actual_time_elapsed > 12.0 && actual_time_elapsed < 13.0) {
				resetTimeAndPressure();
			}
			if (actual_time_elapsed > 30.0) {
				return getCompressionRatio();
			}
			if (std::abs(getEffectiveTemperature() - temperature) > 10) {
				return -1;
			}


		}
		return -1;
	}

	void resetTimeAndPressure() {
		m_accumulated_virial = 0;
		m_time_elapsed = 0;
	}

	double getCompressionRatio() {
		return averagePressure() / Settings::ideal_pressure;
	}

	double getEffectiveTemperature() {
		return m_object_handler.getTemp();
	}

	double getTempUncMax() { return m_temp_uncertainty_max; }

private:
	void update() {
		m_object_handler.update(m_delta_time, m_time_elapsed, m_virial);
		m_display.update();

	}

	void displaySimulationCalculations() {
		std::cout << "\033[10A";
		std::cout << "\033[J";
		std::cout << '\n';

		std::cout << "Pressure exerted this frame: " << instantaneousPressure() << "\n";
		std::cout << "Pressure exerted over the entire simulation duration: " << averagePressure() << '\n';
		std::cout << "Ideal Pressure: " << Settings::ideal_pressure << '\n';
		std::cout << "Real pressure as a percentage of ideal " << percentageOfIdeal() << "%\n";
		std::cout << "Delta time: " << m_delta_time << '\n';
		std::cout << "Time elapsed: " << m_time_elapsed << '\n';
		std::cout << "Virial: " << m_virial << '\n';
		std::cout << "Moles of gas in 1m^3: " << Settings::moles_in_1m3 << '\n';
		std::cout << "Temperature: " << m_object_handler.getTemp() << '\n';
		std::cout << std::flush;

	}

	double instantaneousPressure() {
		return Settings::ideal_pressure + (1 / (3 * Settings::container_volume)) * m_virial;
	}

	double averagePressure() {
		return Settings::ideal_pressure + (1 / (3 * Settings::container_volume)) * m_accumulated_virial / m_time_elapsed;
	}

	double percentageOfIdeal() {
		return averagePressure() * 100 / Settings::ideal_pressure;
	}

	void getUserInput() {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			if (!m_key_pressed) {
				m_delta_time /= 2.0;
				m_key_pressed = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			if (!m_key_pressed) {
				m_delta_time *= 2.0;
				m_key_pressed = true;
			}
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::K)) {
			if (!m_key_pressed) {
				m_accumulated_virial = 0;
				m_time_elapsed = 0;
				m_key_pressed = true;
			}
		}
		else {
			m_key_pressed = false;
		}
	}

	double m_delta_time;
	double m_time_elapsed;
	double m_virial;
	double m_accumulated_virial;
	double m_temp_uncertainty_max;


	bool m_key_pressed;

	ObjectHandler m_object_handler;
	Display m_display;
};

