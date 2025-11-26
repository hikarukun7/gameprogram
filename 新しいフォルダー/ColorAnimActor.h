#pragma once

#include "Actor.h"

class ColorAnimActor : public Actor
{
public:
	ColorAnimActor(class Scene* scene,
		float r = 0.0f, float g = 0.0f, float b = 0.0f,
		float vr = 0.0f, float vg = 0.0f, float cb = 0.0f);
	~ColorAnimActor();

	void update(float deltaTime) override;
	void draw() override;

	void setColor(float r, float g, float b);
	const float* getColor() const { return m_color; }
	void setColorVel(float vr, float vg, float vb);
	const float* getColorvel() const { return m_colorVel; }

private:
	float m_color[3];
	float m_colorVel[3];

};