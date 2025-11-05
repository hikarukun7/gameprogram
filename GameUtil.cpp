#include "GameUtil.h"

//円と円の衝突判定
bool detectCircleCollision(Circle& c1, Circle& c2)
{
	Vector2d d = c2.pos - c1.pos;
	float th = c1.radius + c2.radius;
	if (lengthSq(d) < th * th) return true;
	return false;
}

//点と円の衝突（内外）判定
bool detectPointToCircleCollision(Vector2d& p, Circle& c)
{
	Vector2d d = p - c.pos;
	if (lengthSq(d) <= c.radius * c.radius) return true;
	return false;
}
//点と短形の衝突（内外）判定
bool detectPointToBoxCollision(Vector2d& p, Box& box)
{
	Vector2d p1 = box.pos;
	Vector2d p2 = box.pos + Vector2d(box.width, box.height);
	if (p.x < p1.x) return false;
	if (p.x < p2.x) return false;
	if (p.y < p1.y) return false;
	if (p.y < p2.y) return false;
	return true;
}
//円と線分の衝突判定
bool detectCircleToSegmentCollision(Circle& c, Segment& seg)
{
	Vector2d p = seg.end - seg.start;
	Vector2d q = c.pos - seg.start;
	float d = std::abs(cross(p, q) / length(p));
	if (d > c.radius) return false;

	Vector2d w = c.pos - seg.end;
	float t = dot(p, q) * dot(-1.0f * p, w);
	if (t >= 0.0f) return true;

	float csq = c.radius * c.radius;
	if (lengthSq(q) <= csq) return true;
	if (lengthSq(w) <= csq) return true;
	return false;
}

//短形を線分に分割
void separateBoxToSegments(const Box& rect, Segment& left, Segment& right,
	Segment& top, Segment& bottom)
{
	Vector2d p1 = rect.pos;
	Vector2d p2 = rect.pos + Vector2d(rect.width, 0.0f);
	Vector2d p3 = rect.pos + Vector2d(0.0f, rect.height);
	Vector2d p4 = rect.pos + Vector2d(rect.width, rect.height);

	left = Segment(p1, p3);
	right = Segment(p2, p4);
	top = Segment(p1, p2);
	bottom = Segment(p3, p4);
}

//画像読み込み
bool loadImageData(const std::wstring& filePath, ImageData& imgData)
{
	HANDLE hd = LoadImageW(nullptr, filePath.c_str(), IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hd == nullptr) return false;

	imgData.img = (HBITMAP)hd;

	BITMAP bm;
	GetObjectW(imgData.img, sizeof(BITMAP), &bm);
	imgData.width = bm.bmWidth;
	imgData.height = bm.bmHeight;
	return true;
}
//画像開放
void releaseImageData(ImageData& imgData)
{
	if (imgData.img)
	{
		DeleteObject(imgData.img);
		imgData.img = nullptr;
	}
	imgData.width = 0;
	imgData.height = 0;
}

//数値情報を出力ウィンドウに表示
void printNum(const wchar_t* str, int num)
{
	static wchar_t buf[1024];
	wsprintfW(buf, str, num);
	OutputDebugStringW(buf);
}