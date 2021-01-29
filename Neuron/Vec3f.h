#ifndef VEC3F_H
#define VEC3F_H

#include "NeuronSim/Constants.h"

class Vec3f
{
public:
	inline Vec3f() : x(0.0), y(0.0), z(0.0) {}
	inline Vec3f(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
	inline Vec3f(const Vec3f & other) : x(other.x), y(other.y), z(other.z) {}

	inline float lengthSq() const { return x * x + y * y + z * z; }
	inline float length() const { return sqrtf(lengthSq()); }
	inline void normalize()
	{
		float l = length();
		x /= l;
		y /= l;
		z /= l;
	}
	inline Vec3f normal() const { Vec3f result(*this); result.normalize(); return result; }

	inline float & operator[](int index) { return v[index]; }
	inline const float & operator[](int index) const { return v[index]; }

public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

inline Vec3f operator+(const Vec3f & left, const Vec3f & right) { return Vec3f(left.x + right.x, left.y + right.y, left.z + right.z); }
inline Vec3f & operator+=(Vec3f & left, const Vec3f & right) { left.x += right.x; left.y += right.y; left.z += right.z; return left; }
inline Vec3f operator-(const Vec3f & left, const Vec3f & right) { return Vec3f(left.x - right.x, left.y - right.y, left.z - right.z); }
inline Vec3f operator-(const Vec3f & vec) { return Vec3f(-vec.x, -vec.y, -vec.z); }
inline Vec3f & operator-=(Vec3f & left, const Vec3f & right) { left.x -= right.x; left.y -= right.y; left.z -= right.z; return left; }
inline Vec3f operator*(float scalar, const Vec3f & vec) { return Vec3f(vec.x * scalar, vec.y * scalar, vec.z * scalar); }
inline Vec3f operator*(const Vec3f & vec, float scalar) { return Vec3f(vec.x * scalar, vec.y * scalar, vec.z * scalar); }
inline Vec3f operator*=(Vec3f & vec, float scalar) { vec.x *= scalar; vec.y *= scalar; vec.z *= scalar; return vec; }
inline Vec3f operator/(const Vec3f & vec, float scalar) { return Vec3f(vec.x / scalar, vec.y / scalar, vec.z / scalar); }
inline Vec3f operator/=(Vec3f & vec, float scalar) { vec.x /= scalar; vec.y /= scalar; vec.z /= scalar; return vec; }
inline bool operator==(const Vec3f & left, const Vec3f & right) { return left.x == right.x && left.y == right.y && left.z == right.z; }
inline std::ostream & operator<<(std::ostream & os, const Vec3f & vec) { os << "(" << vec.x << "," << vec.y << "," << vec.z << ")"; return os; }

inline float dot(const Vec3f & left, const Vec3f & right) { return left.x * right.x + left.y * right.y + left.z * right.z; }
inline Vec3f cross(const Vec3f & left, const Vec3f & right)
{
	Vec3f result;
	result.x = left.y * right.z - left.z * right.y;
	result.y = right.x * left.z - right.z * left.x;
	result.z = left.x * right.y - left.y * right.x;
	return result;
}
inline bool approxEqual(const Vec3f & left, const Vec3f & right) { return (right - left).lengthSq() < TINY_VALUE; }

#endif
