#pragma once

#include "GameUtil.h"
#include "Keyboard.h"
#include <Windows.h>
#include <vector>
#include <memory>

class Actor
{
public:
	Actor(class Scene* scene);
	virtual ~Actor();

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;
	void setDead() { m_dead = true; }
	bool isDead() const { return m_dead; }
	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }

	void setMaxSpeed(float speed) { m_maxSpeed = speed; }
	float getMaxSpeed() const { return m_maxSpeed; }

protected:
	class Scene* m_scene;
	bool m_dead;
	bool m_visible;

	float m_maxSpeed;

};

void updateActors(std::vector<Actor*>& actors, float deltaTime);
void drawActors(std::vector<Actor*>& actors);
void releaseActors(std::vector<Actor*>& actors);
void removeActors(std::vector<Actor*>& actors);