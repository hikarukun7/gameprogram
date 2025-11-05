#include "TestScene.h"
#include "Game.h"
#include <exception>

TestScene::TestScene(Game* game)
    : Scene(game)
{
    m_isRunning = true;
}

TestScene::~TestScene()
{
}

void TestScene::update(float deltaTime)
{
}

void TestScene::draw()
{
}