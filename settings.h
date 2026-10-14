#pragma once
#include "collisionInformation.h"

struct vector2 {
	int x;
	int y;
};

namespace Settings {
	constexpr double avo_constant{ 6.02214e23 };
	constexpr double boltz_constant{ 1.380649e-23 };
	constexpr int display_width{ 1920 };
	constexpr int display_height{ 900 };
	constexpr double display_radius{ 20 };

	constexpr double moles_of_gas{ 3.22710e-22 * 4 };
	constexpr double molar_mass{ 0.039948 }; // in kg/mol

	inline double temperature{ 298.15 };
	inline double delta_time{ 1e-15 };
	constexpr double gas_constant{ 8.3145 };
	constexpr double pi{ 3.1415926 };
	inline double avg_speed{ std::sqrt(3 * gas_constant * temperature / molar_mass) };
	constexpr double num_grains{ moles_of_gas * avo_constant };

	constexpr double volume_per_particle{ 2.07e-29 }; // in m^3
	constexpr double van_der_waals_radius{ 1.88e-10 };
	constexpr double critical_temperature{ 150.7 };
	constexpr double particles_per_grain{ moles_of_gas * avo_constant / num_grains };
	constexpr double volume_per_grain{ particles_per_grain * volume_per_particle };
	constexpr double LJ_sigma{ 3.405e-10 };
	constexpr double LJ_epsilon{ 119.8 * boltz_constant };
	const double radius_per_grain{ std::pow(3 * volume_per_grain / (4 * pi), 1 / 3.0) };

	constexpr double actual_container_width{ 2e-8 };
	constexpr double actual_container_height{ 2e-8 };
	constexpr double actual_container_depth{ 2e-8 };

	const double container_width{ actual_container_width };
	const double container_height{ actual_container_height };
	const double container_depth{ actual_container_depth };

	const double actual_container_volume = actual_container_width * actual_container_height * actual_container_depth;

	const double container_volume = container_width * container_height * container_depth;

	const double LJ_sigma_grain{ LJ_sigma * std::pow(particles_per_grain, 1 / 3.0) };
	const double LJ_epsilon_grain{ LJ_epsilon * particles_per_grain };
	constexpr double mass_of_particle{ molar_mass / avo_constant };
	constexpr double mass_of_grain{ particles_per_grain * mass_of_particle };

	inline double ideal_pressure = { moles_of_gas * avo_constant * boltz_constant * temperature / actual_container_volume };

	const double container_area{ 2 * container_width * container_height
	+ 2 * container_depth * container_height
	+ 2 * container_width * container_depth };

	const double moles_in_1m3{ moles_of_gas / container_volume };

	inline void recalculateConstants() {
		ideal_pressure = { moles_of_gas * avo_constant * boltz_constant * temperature / actual_container_volume };
		avg_speed = std::sqrt(3 * gas_constant * temperature / molar_mass);
	}
}


