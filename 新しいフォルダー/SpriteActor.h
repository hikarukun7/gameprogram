#pragma once

#include "Actor2d.h"

class SpriteActor : public Actor2d
{
public:
	SpriteActor(class Scene* scene, const std::wstring& filePath,
		const Vector2d& pos = ZeroVec2d, const Vector2d& vel = ZeroVec2d,
		bool centerFlag = false);
	virtual ~SpriteActor();

	virtual void update(float deltaTime) override;
	virtual void draw() override;

	bool isEnabled() const { return m_enabled; }

protected:
	std::wstring m_imgPath;
	bool m_enabled;
	Vector2d m_offset;
};