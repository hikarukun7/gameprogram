#pragma once

#include <Windows.h>
#include "GMath.h"
#include "GameUtil.h"
#include "Keyboard.h"

#include <memory>
#include "Scene.h"
#include "Renderer.h"

class Game
{
public:
	Game();
	~Game();

	void initialize(HWND hwnd, int width, int height);
	bool loop();

	HWND getHwnd() { return m_hwnd; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }

	const Keyboard& getKeyboard() const { return m_keyboard; }
	Renderer* getRenderer() { return m_rederer.get(); }

private:
	HWND m_hwnd;
	int  m_width;
	int  m_height;

	Keyboard m_keyboard;

	std::unique_ptr<Scene> m_scene;
	std::unique_ptr<Renderer> m_rederer;
	

	static const float FrameRate;
	static const float MaxDeltaTime;

	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;
	LARGE_INTEGER m_freqTime;

	

	void input();
	void update(float deltaTime);
	void draw();

	bool tick(float& deltaTime);

	

};


