#pragma once

#include <cmath>

#include <DirectXMath.h>
using namespace DirectX;

//2ŽŸŒ³ƒxƒNƒgƒ‹
struct Vector2d
{
	float x;
	float y;

	Vector2d() : x(0.0f), y(0.0f) {}
	Vector2d(float ix, float iy) : x(ix), y(iy) {}
	Vector2d(const Vector2d& v) : x(v.x), y(v.y) {}

	Vector2d operator +(const Vector2d& a) const;
	Vector2d operator -(const Vector2d& a) const;
	Vector2d operator *(float a) const;
	Vector2d operator +=(const Vector2d& a);
	Vector2d operator -=(const Vector2d& a);
	Vector2d operator *=(float a);
};

Vector2d operator *(float a, const Vector2d& b);

float length(const Vector2d& v);
float lengthSq(const Vector2d& v);
Vector2d normalize(const Vector2d& v);

Vector2d unitVecAngle(float theta);
float deg2rad(float theta);
float rad2deg(float theta);

float dot(const Vector2d& a, const Vector2d& b);
float cross(const Vector2d& a, const Vector2d& b);

extern const Vector2d ZeroVec2d;
extern const Vector2d Ones2d;
extern const Vector2d UnitVecX2d;
extern const Vector2d UnitVecY2d;

extern const float PI;



