#include "Keyboard.h"

Keyboard::Keyboard()
{
	initialize();
}

Keyboard::~Keyboard()
{
}

void Keyboard::initialize()
{
	memset(m_keyStates, 0, KeyNum * sizeof(BYTE));
	memset(m_keyOldStates, 0, KeyNum * sizeof(BYTE));
}

void Keyboard::input()
{
	memcpy(m_keyOldStates, m_keyStates, KeyNum * sizeof(BYTE));
	BOOL ret = GetKeyboardState(m_keyStates);
	if (!ret)
	{
		initialize();
	}
}

bool Keyboard::isPressed(BYTE key) const
{
	if (m_keyOldStates[key] & 0x80) return false;
	if (m_keyStates[key] & 0x80) return true;
	return false;
}

bool Keyboard::isReleased(BYTE key) const
{
	if (m_keyStates[key] & 0x80) return false;
	if (m_keyOldStates[key] & 0x80) return true;
	return false;
}

bool Keyboard::isDown(BYTE key) const
{
	if (m_keyStates[key] & 0x80) return true;
	return false;
}

bool Keyboard::isUp(BYTE key) const
{
	if (m_keyStates[key] & 0x80) return false;
	return true;
}