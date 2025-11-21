#include "SpriteActor.h"
#include "Game.h"
#include "Scene.h"

SpriteActor::SpriteActor(Scene* scene, const std::wstring& filePath,
	const Vector2d& pos, const Vector2d& vel, bool centerFlag)
	: Actor2d(scene, pos, vel)
	, m_imgPath(filePath)
	, m_enabled(false)
	, m_offset(ZeroVec2d)
{

}
SpriteActor::~SpriteActor()
{
}

void SpriteActor::update(float deltaTime)
{
	m_pos = m_vel * deltaTime;
}

void SpriteActor::draw()
{
}
