#pragma once

class Vector3
{

public:
	Vector3();
	Vector3(float value);
	Vector3(float x, float y, float z);
	~Vector3();
	Vector3(const Vector3& rhs);
	Vector3& operator=(const Vector3& rhs);
	Vector3 operator+(const Vector3& rhs) const;
	Vector3 operator-(const Vector3& rhs) const;
	Vector3 operator*(const float& scalar) const;
	Vector3 operator/(const float& scalar) const;
	void operator+=(const Vector3& rhs);
	void operator-=(const Vector3& rhs);
	void operator*=(const float& scalar);
	void operator/=(const float& scalar);

	void addScaledVector(const Vector3& vector, float scalar);

	void Normalize();
	float Magnitude();
	Vector3 Inverse();

	float x;
	float y;
	float z;
};