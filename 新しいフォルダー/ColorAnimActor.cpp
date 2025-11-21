#include "ColorAnimActor.h"
#include "Scene.h"
#include "Game.h"

ColorAnimActor::ColorAnimActor(Scene* scene,
	float r, float g, float b, float vr, float vg, float vb)
	: Actor(scene)
	, m_color{ r, g, b }
	, m_colorVel{ vr, vg, vb }
{
}

ColorAnimActor::~ColorAnimActor()
{
}

void ColorAnimActor::update(float deltaTime)
{
	m_color[0] += deltaTime * m_colorVel[0];
	m_color[1] += deltaTime * m_colorVel[1];
	m_color[2] += deltaTime * m_colorVel[2];

	//redê¨ï™
	if (m_color[0] < 0.0f)
	{
		m_color[0] = 0.0f;
		m_colorVel[0] *= -1.0f;
	}
	if (m_color[0] > 1.0f)
	{
		m_color[0] = 1.0f;
		m_colorVel[0] *= -1.0f;
	}

	//greenê¨ï™
	if (m_color[1] < 0.0f)
	{
		m_color[1] = 0.0f;
		m_colorVel[1] *= -1.0f;
	}
	if (m_color[1] > 1.0f)
	{
		m_color[1] = 1.0f;
		m_colorVel[1] *= -1.0f;
	}
	//blueê¨ï™
	if (m_color[2] < 0.0f)
	{
		m_color[2] = 0.0f;
		m_colorVel[2] *= -1.0f;
	}
	if (m_color[2] > 1.0f)
	{
		m_color[2] = 1.0f;
		m_colorVel[2] *= -1.0f;
	}
	m_scene->getGame()->getRenderer()->setBackColor(
		m_color[0], m_color[1], m_color[2]);
}

void ColorAnimActor::draw()
{
}

void ColorAnimActor::setColor(float r, float g, float b)
{
	m_color[0] = r;
	m_color[1] = g;
	m_color[2] = b;
}

void ColorAnimActor::setColorVel(float vr, float vg, float vb)
{
	m_colorVel[0] = vr;
	m_colorVel[1] = vg;
	m_colorVel[2] = vb;
	
}