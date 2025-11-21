#pragma once

#include <Windows.h>

class Keyboard
{
public:
	Keyboard();
	~Keyboard();

	void initialize();
	void input();

	bool isPressed(BYTE key) const;
	bool isReleased(BYTE key) const;
	bool isDown(BYTE key) const;
	bool isUp(BYTE key) const;

private:
	static const int KeyNum = 256;

	BYTE m_keyStates[KeyNum];
	BYTE m_keyOldStates[KeyNum];
};