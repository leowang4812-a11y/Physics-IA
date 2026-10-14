#pragma once
#include <cmath>
#include <tuple>
template <typename T = double>
struct Vector3T {
	T x = 0;
	T y = 0;
	T z = 0;

	Vector3T<int> toInt() {
		return { static_cast<int>(x),
				 static_cast<int>(y),
				 static_cast<int>(z) };
	}

	double getMagnitude() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	double getMagnitudeSquared() const{
		return x * x + y * y + z * z;
	}

	Vector3T<T> normalised() {
		return *this / getMagnitude();
	}
	
	bool operator<(const Vector3T<T>& other) const {
		return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
	}

};

using Vector3 = Vector3T<double>;
using Vector3I = Vector3T<int>;


template <typename T>
inline Vector3T<T> operator-(Vector3T<T> vec) {
	return { -vec.x, -vec.y, -vec.z };
}

template <typename T>
inline Vector3T<T> operator+(Vector3T<T> vec1, Vector3T<T> vec2) {
	return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

template <typename T>
inline Vector3T<T> operator+=(Vector3T<T>& vec1, Vector3T<T> vec2) {
	vec1 = vec1 + vec2;
	return vec1;
}

template <typename T>
inline Vector3T<T> operator-(Vector3T<T> vec1, Vector3T<T> vec2) {
	return { vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z };
}

template <typename T>
inline Vector3T<T> operator-=(Vector3T<T>& vec1, Vector3T<T> vec2) {
	vec1 = vec1 + vec2;
	return vec1;
}

template <typename T, typename U>
inline Vector3T<T> operator*(Vector3T<T> vec, U value) {
	return { vec.x * value, vec.y * value, vec.z * value };
}

template <typename T, typename U>
inline Vector3T<T> operator/(Vector3T<T> vec, U value) {
	return { vec.x / value, vec.y / value, vec.z / value };
}

template <typename T, typename U>
inline Vector3T<T> operator*(U value, Vector3T<T> vec ) {
	return vec * value;
}

template <typename T, typename U>
inline Vector3T<T> operator*=(Vector3T<T>& vec, U value) {
	vec = vec * value;
	return vec;
}

template <typename T>
inline bool operator==(const Vector3T<T>& vec1, const Vector3T<T>& vec2) {
	return (vec1.x == vec2.x && vec1.y == vec2.y && vec1.z == vec2.z);
}
template <typename T>
inline T dot(Vector3T<T> vec1, Vector3T<T> vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

