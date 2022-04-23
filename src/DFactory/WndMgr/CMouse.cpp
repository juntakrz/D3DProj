#include "../../pch.h"
#include "CMouse.h"

std::pair<int16_t, int16_t> CMouse::GetPos() const noexcept
{
	return { m_PosX, m_PosY };
}

int16_t CMouse::GetPosX() const noexcept
{
	return m_PosX;
}

int16_t CMouse::GetPosY() const noexcept
{
	return m_PosY;
}

bool CMouse::IsInsideWnd() const noexcept
{
	return m_IsInsideWnd;
}

bool CMouse::IsLMB() const noexcept
{
	return m_LPressed;
}

bool CMouse::IsRMB() const noexcept
{
	return m_RPressed;
}

bool CMouse::IsMMB() const noexcept
{
	return m_MPressed;
}

CMouse::Event CMouse::GetEvent() noexcept
{
	if (m_EventBuffer.size() > 0)
	{
		CMouse::Event CEvent = m_EventBuffer.front();	//get first event in queue
		m_EventBuffer.pop();							//remove it from queue
		return CEvent;									//return the event
	}
	else
	{
		return CMouse::Event();
	}
}

bool CMouse::IsBufferEmpty() const noexcept
{
	return m_EventBuffer.empty();
}

void CMouse::FlushEvents() noexcept
{
	m_EventBuffer = std::queue<Event>();
}

void CMouse::ToggleCursor() noexcept
{
	switch (m_ShowCursor)
	{
	case true:
	{
		while (ShowCursor(false) >= 0);
		m_ShowCursor = false;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;

		// store cursor position
		POINT p;
		GetCursorPos(&p);
		m_oldPosX = p.x;
		m_oldPosY = p.y;
		break;
	}
	case false:
	{
		ShowCursor(true);
		m_ShowCursor = true;
		ImGui::GetIO().ConfigFlags ^= ImGuiConfigFlags_NoMouse;
		SetCursorPos(m_oldPosX, m_oldPosY);		// retrieve cursor position
		break;
	}
	}
}

bool CMouse::IsCursorOn() const noexcept
{
	return m_ShowCursor;
}

void CMouse::RestorePos() const noexcept
{
	SetCursorPos(m_oldPosX, m_oldPosY);
}

void CMouse::GetStoredPos(int16_t& outX, int16_t& outY) const noexcept
{
	outX = m_oldPosX;
	outY = m_oldPosY;
}

void CMouse::OnMove(const int16_t& x, const int16_t& y) noexcept
{
	m_PosX = x, m_PosY = y;
	m_EventBuffer.push(Event(Event::Type::Move, *this));
	TrimBuffer();
}

void CMouse::OnEnter() noexcept
{
	m_IsInsideWnd = true;
	m_EventBuffer.push(Event(Event::Type::Enter, *this));
	TrimBuffer();
}

void CMouse::OnLeave() noexcept
{
	m_IsInsideWnd = false;
	m_EventBuffer.push(Event(Event::Type::Leave, *this));
	TrimBuffer();
}

void CMouse::OnLPress() noexcept
{
	m_LPressed = true;
	m_EventBuffer.push(Event(Event::Type::LPress, *this));
	TrimBuffer();
}

void CMouse::OnLRelease() noexcept
{
	m_LPressed = false;
	m_EventBuffer.push(Event(Event::Type::LRelease, *this));
	TrimBuffer();
}

void CMouse::OnRPress() noexcept
{
	m_RPressed = true;
	m_EventBuffer.push(Event(Event::Type::RPress, *this));
	TrimBuffer();
}

void CMouse::OnRRelease() noexcept
{
	m_RPressed = false;
	m_EventBuffer.push(Event(Event::Type::RRelease, *this));
	TrimBuffer();
}

void CMouse::OnMPress() noexcept
{
	m_MPressed = true;
	m_EventBuffer.push(Event(Event::Type::MPress, *this));
	TrimBuffer();
}

void CMouse::OnMRelease() noexcept
{
	m_MPressed = false;
	m_EventBuffer.push(Event(Event::Type::MRelease, *this));
	TrimBuffer();
}

void CMouse::OnWheelUp() noexcept
{
	m_EventBuffer.push(Event(Event::Type::WheelUp, *this));
	TrimBuffer();
}

void CMouse::OnWheelDown() noexcept
{
	m_EventBuffer.push(Event(Event::Type::WheelDown, *this));
	TrimBuffer();
}

void CMouse::OnWheelDelta(const int16_t& delta) noexcept
{
	m_AccumWheelDelta += delta;

	if (m_AccumWheelDelta >= WHEEL_DELTA)
	{
		m_AccumWheelDelta -= WHEEL_DELTA;
		OnWheelUp();
	}
	if (m_AccumWheelDelta <= -WHEEL_DELTA)
	{
		m_AccumWheelDelta += WHEEL_DELTA;
		OnWheelDown();
	}
}

void CMouse::TrimBuffer() noexcept
{
	while (m_EventBuffer.size() > m_BufferSize)
	{
		m_EventBuffer.pop();
	}
}