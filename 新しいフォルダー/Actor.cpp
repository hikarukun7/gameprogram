#include "Actor.h"
#include "Scene.h"

Actor::Actor(Scene* scene)
    : m_scene(scene)
    , m_dead(false)
    , m_visible(true)
    , m_maxSpeed(0.0f)
{
}

Actor::~Actor()
{
}

void updateActors(std::vector<Actor*>& actors, float deltaTime)
{
    for (auto it = actors.begin(); it != actors.end(); ++it)
    {
        (*it)->update(deltaTime);
    }
}

void drawActors(std::vector<Actor*>& actors)
{
    for (auto it = actors.begin(); it != actors.end(); ++it)
    {
        (*it)->draw();
    }
}

void releaseActors(std::vector<Actor*>& actors)
{
    for (auto it = actors.begin(); it != actors.end(); ++it)
    {
        delete (*it);
    }
    actors.clear();
}

void removeActors(std::vector<Actor*>& actors)
{
    auto it = actors.begin();
    while (it != actors.end())
    {
        if ((*it)->isDead())
        {
            delete (*it);
            it = actors.erase(it);
        }
        else
        {
            ++it;
        }
    }
}