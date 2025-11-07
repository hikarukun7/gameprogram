#pragma once

#include "Actor.h"

class Actor2d : public Actor
{
public:
	Actor2d(class Scene* scene, const Vector2d& pos, const Vector2d& vel);
	virtual ~Actor2d();

	void setPos(Vector2d p) { m_pos = p; }
	Vector2d getPos() const { return m_pos; }
	void setVel(Vector2d v) { m_vel = v; }
	Vector2d getVel() const { return m_vel; }

protected:
	Vector2d m_pos;
	Vector2d m_vel;

};