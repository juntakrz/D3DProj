#pragma once

class CMouse
{
	friend class WndMgr;

public:
	class Event
	{
		uint8_t m_Type;
		bool m_LPressed, m_RPressed, m_MPressed;
		int16_t m_PosX, m_PosY;

	public:
		enum Type
		{
			LPress = 0, LRelease, RPress, RRelease, MPress, MRelease, WheelUp, WheelDown, Move, Enter, Leave, Invalid
		};

		Event() noexcept;
		Event(uint8_t type, CMouse& parent) noexcept;

		bool IsValid() const noexcept;
		uint8_t GetType() const noexcept;
		std::pair<int16_t, int16_t> GetPos() const noexcept;
		int16_t GetPosX() const noexcept;
		int16_t GetPosY() const noexcept;
		bool IsLMB() const noexcept;
		bool IsRMB() const noexcept;
		bool IsMMB() const noexcept;

	};

	CMouse() = default;
	CMouse(const CMouse&) = delete;
	CMouse& operator=(const CMouse&) = delete;

	std::pair<int16_t, int16_t> GetPos() const noexcept;
	int16_t GetPosX() const noexcept;
	int16_t GetPosY() const noexcept;
	bool IsInsideWnd() const noexcept;
	bool IsLMB() const noexcept;
	bool IsRMB() const noexcept;
	bool IsMMB() const noexcept;

	CMouse::Event GetEvent() noexcept;
	bool IsBufferEmpty() const noexcept;
	void FlushEvents() noexcept;

	void ToggleCursor() noexcept;
	bool IsCursorOn() const noexcept;
	void RestorePos() const noexcept;
	void GetStoredPos(int16_t& outX, int16_t& outY) const noexcept;
	
private:
	HWND m_hWnd;
	bool m_LPressed = false, m_RPressed = false, m_MPressed = false;
	bool m_IsInsideWnd = false, m_ShowCursor = true;
	int16_t m_PosX, m_PosY, m_oldPosX, m_oldPosY;
	int16_t m_AccumWheelDelta;
	static constexpr uint8_t m_BufferSize = 16u;
	std::queue<Event> m_EventBuffer;

	void OnMove(const int16_t& x, const int16_t& y) noexcept;
	void OnEnter() noexcept;
	void OnLeave() noexcept;
	void OnLPress() noexcept;
	void OnLRelease() noexcept;
	void OnRPress() noexcept;
	void OnRRelease() noexcept;
	void OnMPress() noexcept;
	void OnMRelease() noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void OnWheelDelta(const int16_t& delta) noexcept;
	void TrimBuffer() noexcept;
};