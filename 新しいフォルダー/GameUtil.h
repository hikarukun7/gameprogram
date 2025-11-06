#pragma once

#include "Gmath.h"
#include <Windows.h>
#include <string>

//円の表現
struct Circle
{
	Vector2d pos;
	float    radius;

	Circle() : pos(ZeroVec2d), radius(1.0f) {}
	Circle(Vector2d p, float r) : pos(p), radius(r) {}
};

//短形表現
struct  Box
{
	Vector2d pos;
	float width;
	float height;

	Box() : pos(ZeroVec2d), width(1.0f), height(1.0f) {}
	Box(Vector2d p, float w, float h) : pos(p), width(w), height(h) {}
};

//線分表現
struct Segment
{
	Vector2d start;
	Vector2d end;

	Segment() : start(ZeroVec2d), end(UnitVecX2d) {}
	Segment(const Vector2d& s, const Vector2d& e) : start(s), end(e) {}
};

//点と円の衝突(内外）判定
bool detectPointToCircleCollision(Vector2d& p, Circle& c);
//点と短形の衝突（内外）判定
bool detectPointToBoxCollision(Vector2d& p, Box& box);
//円と円の衝突判定
bool detectCircleCollision(Circle& c1, Circle& c2);
//円と線分の衝突判定
bool detectCircleToSegmentCollision(Circle& c, Segment& seg);

//短形を線分に分割
void separateBoxToSegments(const Box& rect, Segment& left, Segment& right,
	Segment& top, Segment& bottom);

//画像情報構造体
struct ImageData
{
	HBITMAP img;
	int width;
	int height;

	ImageData() : img(nullptr), width(0), height(0) {}
};

//画像読み込み
bool loadImageData(const std::wstring& filePath, ImageData& imgData);
//画像開放
void releaseImageData(ImageData& imgData);
//数値情報を出力ウィンドウに表示
void printNum(const wchar_t* str, int num);