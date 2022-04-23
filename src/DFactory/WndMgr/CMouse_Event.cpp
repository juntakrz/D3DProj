#include "../../pch.h"
#include "CMouse.h"

CMouse::Event::Event() noexcept :
	m_Type(Type::Invalid), m_LPressed(false), m_RPressed(false), m_MPressed(false), m_PosX(0), m_PosY(0) {};

CMouse::Event::Event(uint8_t type, CMouse& parent) noexcept :
	m_Type(type), m_LPressed(parent.m_LPressed), m_RPressed(parent.m_RPressed), m_MPressed(parent.m_MPressed),
	m_PosX(parent.m_PosX), m_PosY(parent.m_PosY) {};

bool CMouse::Event::IsValid() const noexcept
{
	return m_Type != Type::Invalid;
}

uint8_t CMouse::Event::GetType() const noexcept
{
	return m_Type;
}

std::pair<int16_t, int16_t> CMouse::Event::GetPos() const noexcept
{
	return { m_PosX, m_PosY };
}

int16_t CMouse::Event::GetPosX() const noexcept
{
	return m_PosX;
}

int16_t CMouse::Event::GetPosY() const noexcept
{
	return m_PosY;
}

bool CMouse::Event::IsLMB() const noexcept
{
	return m_LPressed;
}

bool CMouse::Event::IsRMB() const noexcept
{
	return m_RPressed;
}

bool CMouse::Event::IsMMB() const noexcept
{
	return m_MPressed;
}

