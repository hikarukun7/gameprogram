#pragma once

#include "Scene.h"
#include "Actor.h"
#include "ColorAnimActor.h"

class TestScene : public Scene
{
public:
	TestScene(class Game* game);
	~TestScene();

	void update(float deltatime) override;
	void draw() override;

private:

};