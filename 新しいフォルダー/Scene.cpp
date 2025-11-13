#include "Scene.h"
#include "Game.h"

Scene::Scene(Game* game)
    : m_game(game)
    , m_isRunning(false)
{
}

Scene::~Scene()
{
}