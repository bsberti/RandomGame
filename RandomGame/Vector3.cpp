#include "Vector3.h"
#include <cmath>

// Default Constructor
// Accepts no parameters
// Returns nothing
Vector3::Vector3()
	: Vector3(0.f)
{

}

// Parameterized Constructor
// Accepts a value to use for all axis
// Returns nothing
Vector3::Vector3(float value)
	: Vector3(value, value, value)
{

}

// Default Constructor
// Accepts no parameters
// Returns nothing
Vector3::Vector3(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{

}

// Destructor
// Accepts no parameters
// Returns nothing
Vector3::~Vector3() {

}

// Copy Constructor
// Accepts a reference to another Vector3
// returns nothing
Vector3::Vector3(const Vector3& rhs)
//: x(other.x)		This is another option instead
//, y(other.y)		of assigning all of the values
//, z(other.z)		within the constructor.
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}

// Set
// Sets the vector values to the provided values
// returns nothing
void Vector3::Set(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

// Assignment Operator Overload
// Accepts a reference to another Vector3
// Returns a refernce to the new Vector3
Vector3& Vector3::operator=(const Vector3& rhs) {
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

// Add Operator Overload
// Accepts a reference to the right hand side of the add operation
// Returns a copy to the new Vector3
Vector3 Vector3::operator+(const Vector3& rhs) const {
	return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

// Add Operator Overload
// Accepts a reference to the right hand side of the add operation
// Returns a copy to the new Vector3
Vector3 Vector3::operator-(const Vector3& rhs) const {
	return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

// Add Operator Overload
// Accepts a reference to the right hand side of the add operation
// Returns a copy to the new Vector3
Vector3 Vector3::operator-() const {
	return Vector3(-x, -y, -z);
}

// Add Operator Overload
// Accepts a reference to the right hand side of the add operation
// Returns a copy to the new Vector3
Vector3 Vector3::operator*(const float& scalar) const {
	return Vector3(x * scalar, y * scalar, z * scalar);
}

// Add Operator Overload
// Accepts a reference to the right hand side of the add operation
// Returns a copy to the new Vector3
Vector3 Vector3::operator/(const float& scalar) const {
	return Vector3(x / scalar, y / scalar, z / scalar);
}

// Add Assignment Operator Overload
// Accepts a reference to the right hand side of the add operation
// no return, values are changed internally
void Vector3::operator+=(const Vector3& rhs) {
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

// Subtract Assignment Operator Overload
// Accepts a reference to the right hand side of the add operation
// no return, values are changed internally
void Vector3::operator-=(const Vector3& rhs) {
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

// Multiply Assignment Operator Overload
// Accepts a reference to the right hand side of the add operation
// no return, values are changed internally
void Vector3::operator*=(const float& scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
}

// Division Assignment Operator Overload
// Accepts a reference to the right hand side of the add operation
// no return, values are changed internally
void Vector3::operator/=(const float& scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
}

// Add Scaled Vector
// Adds a vector that is scaled by a provided scalar to
// this vector.
void Vector3::addScaledVector(const Vector3& vector, float scalar)
{
	*this += vector * scalar;
}

// Normalize changes the values of the vector to be 
// a unit length
void Vector3::Normalize() {
	float m = Magnitude();
	x /= m;
	y /= m;
	z /= m;
}

// Magnitude will return the length of the vector
float Vector3::Magnitude() {
	return sqrt(x * x + y * y + z * z);
}
// Inverse will invert the direction of the vector
Vector3 Vector3::Inverse() {
	// this  <-- pointer
	// *this <-- dereferenced pointer
	// this = Vector3*   pointer of type
	// *this = Vector3   type
	//return *this * -1.f;

	return Vector3(x, y, z) * -1.f;
}

// Multiply Operator Overload
// Accepts a reference to the left hand side of the operation
// Returns a copy to the new Vector3
Vector3 operator*(const float& scalar, const Vector3& vector) {
	return Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}