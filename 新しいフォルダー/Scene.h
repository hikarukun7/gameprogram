#pragma once

class Scene
{
public:
	Scene(class Game* game);
	virtual ~Scene();

	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;

	class Game* getGame() { return m_game; }
	bool isRunning() const { return m_isRunning; }

protected:
	class Game* m_game;
	bool m_isRunning;
};