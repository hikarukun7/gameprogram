#include "Gmath.h"
#include <cfloat>

const Vector2d ZeroVec2d(0.0f, 0.0f);
const Vector2d Ones2d(1.0f, 1.0f);
const Vector2d UnitVecX2d(1.0f, 0.0f);
const Vector2d UnitVecY2d(0.0f, 1.0f);

const float PI = 3.14159265f;

Vector2d Vector2d::operator +(const Vector2d& a) const
{
	return Vector2d(this->x + a.x, this->y + a.y);
}

Vector2d Vector2d::operator -(const Vector2d& a) const
{
	return Vector2d(this->x - a.x, this->y - a.y);
}

Vector2d Vector2d::operator *(float a) const
{
	return Vector2d(this->x * a, this->y * a);
}

Vector2d Vector2d::operator +=(const Vector2d& a)
{
	this->x += a.x;
	this->y += a.y;

	return *this;
}

Vector2d Vector2d::operator -=(const Vector2d& a)
{
	this->x -= a.x;
	this->y -= a.y;

	return *this;
}

Vector2d Vector2d::operator *=(float a)
{
	this->x *= a;
	this->y *= a;

	return *this;
}

Vector2d operator *(float a, const Vector2d& b)
{
	return Vector2d(a * b.x, a * b.y);
}

float length(const Vector2d& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

float lengthSq(const Vector2d& v)
{
	return v.x * v.x + v.y * v.y;
}

Vector2d normalize(const Vector2d& v)
{
	float len = length(v);
	if (len < FLT_EPSILON) return Vector2d(0.0f, 0.0f);

	float inLen = 1.0f / len;
	return inLen * v;
}

Vector2d unitVecAngle(float theta)
{
	return Vector2d(std::cos(theta), std::sin(theta));
}

float deg2rad(float theta)
{
	return theta * PI / 180.0f;
}

float rad2deg(float theta)
{
	return theta * 180.0f / PI;
}

float dot(const Vector2d& a, const Vector2d& b)
{
	return a.x * b.x + a.y * b.y;
}

float cross(const Vector2d& a, const Vector2d& b)
{
	return a.x * b.y - a.y * b.x;
}