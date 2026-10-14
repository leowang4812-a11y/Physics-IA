#pragma once
#include "object.h"
#include "vector3.h"
#include "random.h"
#include <vector>
#include <functional>
#include <map>
#include <set>

class ObjectHandler {
public:
	ObjectHandler(int num_objects)
		: m_num_objects{ num_objects } 
		, m_impulse_this_frame{ 0.0 } 
		, m_total_impulse{ 0.0 } 
		, m_num_subdivisions{ 5 } 
		, m_actual_temperature{ 0.0 } {
		randomInit();
		//testInit();
	}

	void update(double delta_time, double time_elapsed, double& virial) {
		m_impulse_this_frame = 0.0;
		for (auto& sphere : m_spheres) {
			sphere.update(delta_time, m_impulse_this_frame);

		}
		partitionIntoCellList();
		IFBetweenAllParticles(virial);
		for (auto& sphere : m_spheres) {
			sphere.finalVelocityUpdate(delta_time);

		}
		if (time_elapsed > 2 * Settings::delta_time) {
			m_total_impulse += m_impulse_this_frame;
		}
		calcTemp();
		heatInjection();
	}

	const std::vector<Sphere>& getSpheres() const { return m_spheres; }
	double getImpulse() const { return m_impulse_this_frame; }
	double getTotalImpulse() const { return m_total_impulse; }
	double getTemp() const { return m_actual_temperature; }

private:
	void calcTemp() {
		double KE = 0.0;
		for (const auto& sphere : m_spheres) {
			KE += sphere.getKineticEnergy();
		}
		m_actual_temperature = (2 / 3.0) * KE / (Settings::moles_of_gas * Settings::gas_constant);
	}

	void heatInjection() {
		if (m_actual_temperature < Settings::temperature) {
			for (auto& sphere : m_spheres) {
				sphere.setVel(sphere.getVel() * (1 + 5e-4));
			}
		}
		else {
			for (auto& sphere : m_spheres) {
				sphere.setVel(sphere.getVel() * (1 - 5e-4));
			}
		}
	}

	Sphere getRandomSphere(std::set<Vector3>& chosen) {
		Vector3 rnv = getRandomNormalisedVector();

		Vector3 pos = { Random::get(Settings::container_width),
			Random::get(Settings::container_height),
			Random::get(Settings::container_depth) };

		while (chosen.count(pos) == 1) {
			pos = { Random::get(Settings::container_width),
				Random::get(Settings::container_height),
				Random::get(Settings::container_depth) };
		}
		chosen.insert(pos);
		Sphere sphere{ 
			pos
			, Settings::avg_speed * rnv
			, Settings::radius_per_grain
			, Settings::mass_of_grain };
		
		return sphere;
	}

	Vector3 getRandomNormalisedVector() {
		while (true) { 
			Vector3 vec = { Random::get(0, 1000) / 1000.0, Random::get(0, 1000) / 1000.0, Random::get(0, 1000) / 1000.0 };
			if (vec.getMagnitudeSquared() <= 1) {
				return vec / vec.getMagnitude();
			}
		}
	}

	void randomInit() {
		std::set<Vector3> chosen{};
		for (int i = 0; i < m_num_objects; ++i) {
			m_spheres.emplace_back(getRandomSphere(chosen));
		}
	}

	void testInit() {
		m_spheres.emplace_back(Sphere({ Settings::container_width /2 , Settings::container_height * (49/50.0), Settings::container_depth / 2}, {0,0,0}, Settings::radius_per_grain, Settings::mass_of_grain));
		m_spheres.emplace_back(Sphere({ Settings::container_width /2, Settings::container_height * (9 / 50.0), Settings::container_depth / 2}, {0,0,0}, Settings::radius_per_grain, Settings::mass_of_grain));
	}

	void IFBetweenParticles(Sphere& s1, Sphere& s2, bool oneway, Vector3 across_boundary, double& virial) {
		Vector3 effective_s2_pos = s2.getPos();
		if (across_boundary == Vector3{ 0.0, 0.0, 0.0 }) {
		}
		else {
			effective_s2_pos += across_boundary;
		}

		Vector3 vec_difference = effective_s2_pos - s1.getPos();
		double distance_squared = vec_difference.getMagnitudeSquared();
		double distance = std::sqrt(distance_squared);
		Vector3 direction_vec = vec_difference / distance;  // point s1 to s2

		

	
		// check collision here
		double gap_between_centres = distance - (s1.getRadius() + s2.getRadius());


		if (distance < Settings::LJ_sigma_grain / 2.0)
			distance = Settings::LJ_sigma_grain / 2.0;

		double force_magnitude_attractive{ 4 * Settings::LJ_epsilon_grain *
			(6 * std::pow(Settings::LJ_sigma_grain / distance, 6) * 1 / distance) };

		double force_magnitude_repulsive{ 4 * Settings::LJ_epsilon_grain *
			(12 * std::pow(Settings::LJ_sigma_grain / distance, 12) * 1 / distance) };

		Vector3 force_recieved = direction_vec * (force_magnitude_attractive - force_magnitude_repulsive);

		s1.recieveForce(force_recieved);

//with repuslive component of Lennard Jones potential

		virial -= 0.5 * dot(force_recieved, vec_difference);

		if (!oneway) {
			s2.recieveForce(-force_recieved);
			virial -= 0.5 * dot(force_recieved, vec_difference);
		}
	}
	
		/*if (gap_between_centres < 0) {

			s1.adjustPos(direction_vec * gap_between_centres / 2);
			s2.adjustPos(-direction_vec * gap_between_centres / 2);

			Vector3 tang1 = dot(s1.getVel(), direction_vec) * direction_vec;
			Vector3 tang2 = dot(s2.getVel(), direction_vec) * direction_vec;

			Vector3 norm1 = s1.getVel() - tang1;
			Vector3 norm2 = s2.getVel() - tang2;

			// this assumes particles are the same mass
			s1.setVel(norm1 + tang2);
			s2.setVel(norm2 + tang1);
		}

		if (gap_between_centres < 0) {
			distance = s1.getRadius() + s2.getRadius();
		}*/

	void IFBetweenAllParticles(double& virial) {
		int count = 0;
		for (auto& pair : m_cell_list) {
			std::vector<std::reference_wrapper<Sphere>> all_cells_adjacent;
			std::vector<Vector3> boundary_movement;

			std::vector<int> choices{ -1, 0, 1 };

			for (int choice1 : choices) {
				for (int choice2 : choices) {
					for (int choice3 : choices) {
						if (choice1 == 0 && choice2 == 0 && choice3 == 0)
							continue;

						auto func = [=](int& value) {
							int value_start = value;
							value %= m_num_subdivisions;
							if (value < 0) value += m_num_subdivisions;

							return value - value_start;
						};

						int x = (pair.first.x + choice1);
						int y = (pair.first.y + choice2);
						int z = (pair.first.z + choice3);

						Vector3 bm{};

						double x_dif = func(x);
						double y_dif = func(y);
						double z_dif = func(z);

						if (x_dif > 0) bm = { -Settings::container_width, 0.0, 0.0 };
						else if (x_dif < 0) bm = { Settings::container_width, 0.0, 0.0 };

						if (y_dif > 0) bm = { 0.0, -Settings::container_height, 0.0 };
						else if (y_dif < 0) bm = { 0.0, Settings::container_height, 0.0 };

						if (z_dif > 0) bm = { 0.0, 0.0, -Settings::container_depth };
						else if (z_dif < 0) bm = { 0.0, 0.0, Settings::container_depth };

						for (auto& sphere_ref : m_cell_list[{ x, y, z }]) {
							all_cells_adjacent.emplace_back(sphere_ref);
							boundary_movement.emplace_back(bm);
						}
					}
				}
			}
			// IF forces in a box
			for (int i = 0; i < pair.second.size(); ++i) {
				for (int j = i + 1; j < pair.second.size(); ++j) {
					IFBetweenParticles(pair.second[i].get(), pair.second[j].get(), false, Vector3{}, virial);
				}
			}
			// IF forces from the surrounding boxes.
			for (int i = 0; i < all_cells_adjacent.size(); ++i) {
				for (int j = 0; j < pair.second.size(); ++j) {
					IFBetweenParticles(pair.second[j].get(), all_cells_adjacent[i].get(), true, boundary_movement[j], virial);
				}
			}
		}
	}

	void partitionIntoCellList() {
		for (auto& [key, value] : m_cell_list) {
			value = std::vector<std::reference_wrapper<Sphere>>{};
		}

		for (auto& sphere : m_spheres) {
			Vector3 vec = sphere.getPos();

			Vector3I vec_cell = Vector3I{ static_cast<int>(vec.x / (Settings::container_width / m_num_subdivisions)),
											static_cast<int>(vec.y / (Settings::container_height / m_num_subdivisions)),
											static_cast<int>(vec.z / (Settings::container_depth / m_num_subdivisions)) };

			m_cell_list[vec_cell].emplace_back(sphere);

		}
	}

	std::vector<Sphere> m_spheres;
	std::map<Vector3I, std::vector<std::reference_wrapper<Sphere>>> m_cell_list;
	int m_num_objects;
	int m_num_subdivisions;
	double m_impulse_this_frame;
	double m_total_impulse;
	double m_actual_temperature;
};

