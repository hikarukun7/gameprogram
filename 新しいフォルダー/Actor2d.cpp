#include "Actor2d.h"
#include "Scene.h"

Actor2d::Actor2d(Scene* scene, const Vector2d& pos, const Vector2d& vel)
    : Actor(scene)
    , m_pos(pos)
    , m_vel(vel)
{
}

Actor2d::~Actor2d()
{
}