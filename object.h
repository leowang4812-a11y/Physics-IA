#pragma once
#include "vector3.h"
#include "settings.h"
#include "iostream"

class Sphere {
public:
	Sphere(Vector3 pos, Vector3 vel, double radius, double mass)
		: m_pos{ pos }
		, m_vel{ vel }
		, m_acc{ Vector3{} }
		, m_radius{ radius } 
		, m_mass{ mass }  {

	}

	void update(double delta_time, double& net_impulse) {
		m_pos += m_vel * delta_time + 0.5 * m_acc * delta_time * delta_time;
		m_vel += m_acc * delta_time / 2;

		wallCollide(net_impulse);


	}

	void finalVelocityUpdate(double delta_time) {
		m_vel += m_acc * delta_time / 2;
		m_acc = Vector3{}; // reset acceleration
	}

	void wallCollide(double& net_impulse) {
		double right = Settings::container_width;
		if (m_pos.x > right) {
			net_impulse += std::abs(2 * m_vel.x * m_mass);
			m_pos.x -= right;
		}

		double left = 0;
		if (m_pos.x < left) {
			net_impulse += std::abs(2 * m_vel.x * m_mass);
			m_pos.x += right;
		}

		double bottom = Settings::container_height;
		if (m_pos.y > bottom) {
			net_impulse += std::abs(2 * m_vel.y * m_mass);
			m_pos.y -= bottom;
		}

		double top = 0;
		if (m_pos.y < top) {
			net_impulse += std::abs(2 * m_vel.y * m_mass);
			m_pos.y += bottom;
		}

		double back = Settings::container_depth;
		if (m_pos.z > back) {
			net_impulse += std::abs(2 * m_vel.z * m_mass);
			m_pos.z -= back;
		}

		double front = 0;
		if (m_pos.z < front) {
			net_impulse += std::abs(2 * m_vel.z * m_mass);
			m_pos.z += back;
		}
		
	}

	void recieveForce(Vector3 force) {
		m_acc += force / m_mass;
	}

	void adjustPos(const Vector3& other) {
		m_pos += other;
	}

	void setVel(const Vector3& other) {
		m_vel = other;
	}

	void reverseVel() {
		m_vel *= -1;
	}


	Vector3 getPos() const { return m_pos; }
	Vector3 getVel() const { return m_vel; }
	double getRadius() const { return m_radius; }
	double getMass() const { return m_mass; }
	double getKineticEnergy() const { return 0.5 * m_mass * m_vel.getMagnitudeSquared(); }

private:
	Vector3 m_pos;
	Vector3 m_vel;
	Vector3 m_acc;

	double m_radius;
	double m_mass;
};

