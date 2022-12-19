#include "PhysicsUtils.h"
#include <glm/geometric.hpp>

int TestSphereSphere(const Vector3&posA, float radiusA, const Vector3&posB, float radiusB)
{
	// Calculate squared distance between centers
	Vector3 d = posA - posB;
	float dist2 = Dot(d, d);

	// Spheres intersect if squared distance is less
	// than squared sum of radii
	float radiusSum = radiusA + radiusB;
	return dist2 <= radiusSum * radiusSum;
}

float SqDistPointAABB(Vector3 p, AABB b)
{
	float sqDist = 0.0f;

	// x, y, z
	// 0, 1, 2

	//for (int i = 0; i < 3; i++) {
	//	// For each axis count any excess distance outside box extents
	//	float v = p[i];
	//	if (v < b.Min[i]) sqDist += (b.Min[i] - v) * (b.Min[i] - v);
	//	if (v > b.Max[i]) sqDist += (v - b.Max[i]) * (v - b.Max[i]);
	//}
	float v;
	v = p.x;
	if (v < b.Min[0]) sqDist += (b.Min[0] - v) * (b.Min[0] - v);
	if (v > b.Max[0]) sqDist += (v - b.Max[0]) * (v - b.Max[0]);

	v = p.y;
	if (v < b.Min[1]) sqDist += (b.Min[1] - v) * (b.Min[1] - v);
	if (v > b.Max[1]) sqDist += (v - b.Max[1]) * (v - b.Max[1]);

	v = p.z;
	if (v < b.Min[2]) sqDist += (b.Min[2] - v) * (b.Min[2] - v);
	if (v > b.Max[2]) sqDist += (v - b.Max[2]) * (v - b.Max[2]);

	return sqDist;
}

int TestSphereAABB(const Vector3 &center, float radius, AABB b)
{
	// Compute squared distance between sphere center and AABB
	float sqDist = SqDistPointAABB(center, b);

	// Sphere and AABB intersect if the (squared) distance
	// between them is less than the (squared) sphere radius
	return sqDist <= radius * radius;
}

Point ClosestPtPointTriangle(Vector3 p, Vector3 a, Vector3 b, Vector3 c)
{
	// Check if P in vertex region outside A
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ap = p - a;
	float d1 = Dot(ab, ap);
	float d2 = Dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return a;
	// Barycentric coordinates (1, 0, 0)
	
	// Check if P in vertex region outside B
	Vector3 bp = p - b;
	float d3 = Dot(ab, bp);
	float d4 = Dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return b;
	// barycentric coordinates (0,1,0)

	// Check if P in edge region of AB, if so return projection of P
	//onto AB
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + v * ab; // barycentric coordinates (1-v, v, 0)
	}

	// Check if P in vertex region outside C
	Vector3 cp = p - c;
	float d5 = Dot(ab, cp);
	float d6 = Dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return c;
	// barycentric coordinates (0, 0, 1)

	// Check if P in edge region of AC, if so return projection of P
	// onto AC
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + w * ac; // barycentric coordinates (1-w, 0, w)
	}

	// Check if P in edge region of BC, if so return projection of P
	// onto BC
		float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b);
		// barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric
	// coordinates(u, v, w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w;
	// = u*a +v*b + w*c, u = va * denom = 1.0f - v - w
}

int TestSphereTriangle(const Point &center, float radius, Point a, Point b, Point c, Point& p)
{
	// Find point P on triangle ABC closest to the sphere center
	p = ClosestPtPointTriangle(center, a, b, c);
	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	Vector3 v = p - center;
	return Dot(v, v) <= radius * radius;
}

// Test if ray r = p + td intersects sphere s
// d must be a normalized vector!!!
int TestRaySphere(const Point& p, const Vector3& d, const Point& center, float radius)
{
	Vector3 m = p - center;
	float c = Dot(m, m) - radius * radius;

	// If there is definitely at least one real root, there must be an intersection
	if (c <= 0.0f) return 1;
	float b = Dot(m, d);

	// Early exit if ray origin outside sphere and ray pointing away from sphere
	if (b > 0.0f) return 0;

	float disc = b * b - c;

	// A negative discriminant corresponds to ray missing sphere
	if (disc < 0.0f) return 0;

	// Now ray must hit sphere
	return 1;
}