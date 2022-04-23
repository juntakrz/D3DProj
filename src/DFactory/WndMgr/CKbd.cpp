#include "../../pch.h"
#include "CKbd.h"

bool CKbd::IsKeyDown(uint8_t keycode) const noexcept
{
	return m_KeyStates[keycode];
}

std::optional<CKbd::Event> CKbd::ReadKey() noexcept
{
	if (m_KeyBuffer.size() > 0)		//if something is in the buffer - return the first 'Event' in queue
	{
		CKbd::Event KEvent = m_KeyBuffer.front();
		m_KeyBuffer.pop();
		return KEvent;
	}

	return {};						//or return a new invalid zero 'Event'
}

bool CKbd::IsKeyBufferEmpty() const noexcept
{
	return m_KeyBuffer.empty();
}

void CKbd::FlushKeyBuffer() noexcept
{
	m_KeyBuffer = std::queue<Event>();
}

wchar_t CKbd::ReadChar() noexcept
{
	if (m_CharBuffer.size() > 0)
	{
		wchar_t charcode = m_CharBuffer.front();
		m_CharBuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

bool CKbd::IsCharEmpty() const noexcept
{
	return m_CharBuffer.empty();
}

void CKbd::FlushCharBuffer() noexcept
{
	m_CharBuffer = std::queue<wchar_t>();
}

void CKbd::Flush() noexcept
{
	FlushKeyBuffer();
	FlushCharBuffer();
}

void CKbd::EnableAutoRepeat() noexcept
{
	m_autoRepeatEnabled = true;
}

void CKbd::DisableAutoRepeat() noexcept
{
	m_autoRepeatEnabled = false;
}

bool CKbd::IsAutoRepeatEnabled() const noexcept
{
	return m_autoRepeatEnabled;
}

void CKbd::OnKeyPress(uint8_t keycode) noexcept
{
	m_KeyStates[keycode] = true;									//mark this key as pressed
	m_KeyBuffer.push(CKbd::Event(Event::Type::Press, keycode));		//construct corresponding event
	TrimBuffer(m_KeyBuffer);										//trim event queue if needed
}

void CKbd::OnKeyRelease(uint8_t keycode) noexcept
{
	m_KeyStates[keycode] = false;
	m_KeyBuffer.push(CKbd::Event(Event::Type::Release, keycode));
	TrimBuffer(m_KeyBuffer);
}

void CKbd::OnGetChar(wchar_t character) noexcept
{
	m_CharBuffer.push(character);
	TrimBuffer(m_CharBuffer);
}

void CKbd::ClearKeyStates() noexcept
{
	m_KeyStates.reset();
}

template <typename T>
static void CKbd::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > m_BufferSize)
	{
		buffer.pop();
	}
}