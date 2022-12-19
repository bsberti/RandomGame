#pragma once
#include <glm/vec3.hpp>

class Vector3
{
	// private area

public:

	// Default Constructor
	// Accepts no parameters
	// Returns nothing
	Vector3();

	Vector3(const glm::vec3& v) : x(v.x), y(v.y), z(v.z) {}

	// Parameterized Constructor
	// Accepts a value to set for all axis
	// Returns nothing
	Vector3(float value);

	// Parameterized Constructor
	// Accepts values for all axis
	// Returns nothing
	Vector3(float x, float y, float z);

	// Destructor
	// Accepts no parameters
	// Returns nothing
	~Vector3();

	// Copy Constructor
	// Accepts a reference to another Vector3
	// returns nothing
	Vector3(const Vector3& rhs);

	const glm::vec3 GetGLM() const
	{
		return glm::vec3(x, y, z);
	}

	// Set
	// Sets the vector values to the provided values
	// returns nothing
	void Set(float newX, float newY, float newZ);

	// Assignment Operator Overload
	// Accepts a reference to another Vector3
	// Returns a refernce to the new Vector3
	Vector3& operator=(const Vector3& rhs);

	// Add Operator Overload
	// Accepts a reference to the right hand side of the operation
	// Returns a copy to the new Vector3
	Vector3 operator+(const Vector3& rhs) const;

	// Minus Operator Overload
	// Accepts a reference to the right hand side of the operation
	// Returns a copy to the new Vector3
	Vector3 operator-(const Vector3& rhs) const;

	// Minus Operator Overload
	// Accepts a reference to the right hand side of the operation
	// Returns a copy to the new Vector3
	Vector3 operator-() const;

	// Multiply Operator Overload
	// Accepts a reference to the right hand side of the operation
	// Returns a copy to the new Vector3
	Vector3 operator*(const float& scalar) const;

	// Division Operator Overload
	// Accepts a reference to the right hand side of the operation
	// Returns a copy to the new Vector3
	Vector3 operator/(const float& scalar) const;

	// Addition assignment Operator Overload
	// Accepts a reference to the right hand side of the operation
	// no return, values are changed internally
	void operator+=(const Vector3& rhs);

	// Subtract assignment Operator Overload
	// Accepts a reference to the right hand side of the operation
	// no return, values are changed internally
	void operator-=(const Vector3& rhs);

	// Multiply assignment Operator Overload
	// Accepts a reference to the right hand side of the operation
	// no return, values are changed internally
	void operator*=(const float& scalar);

	// Division assignment Operator Overload
	// Accepts a reference to the right hand side of the operation
	// no return, values are changed internally
	void operator/=(const float& scalar);

	// Add Scaled Vector
	// Adds a vector that is scaled by a provided scalar to
	// this vector.
	void addScaledVector(const Vector3& vector, float scalar);

	// Normalize changes the values of the vector to be 
	// a unit length
	void Normalize();

	// Magnitude will return the length of the vector
	float Magnitude() const;

	// Distance will return the distance between two vectors
	static float Distance(const Vector3& a, const Vector3& b);

	// Distance will return the distance between two vectors
	static float Length(const Vector3& v);

	// Inverse will invert the direction of the vector
	Vector3 Inverse();

	// Public Variables

	// The value along the x-axis
	float x;

	// The value along the y-axis
	float y;

	// The value along the z-axis
	float z;
};

// Multiply Operator Overload
// Accepts a reference to the left hand side of the operation
// Returns a copy to the new Vector3
Vector3 operator*(const float& scalar, const Vector3& vector);