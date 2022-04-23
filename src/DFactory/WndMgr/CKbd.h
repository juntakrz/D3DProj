#pragma once

class CKbd
{
	friend class WndMgr;
public:
	class Event
	{
		uint8_t m_type;
		wchar_t m_code;
	public:
		enum Type
		{
			Press = 0, Release, Invalid
		};
		Event() noexcept : m_type(Type::Invalid), m_code(0u) {};
		Event(uint8_t type, wchar_t code) noexcept : m_type(type), m_code(code) {};

		bool IsPress() const noexcept;
		bool IsRelease() const noexcept;
		bool IsValid() const noexcept;
		uint8_t GetCode() const noexcept;
	};
	CKbd() = default;
	CKbd(const CKbd&) = delete;
	CKbd& operator=(const CKbd&) = delete;

	CKbd* Get()
	{
		return this;
	}

	//key events
	bool IsKeyDown(uint8_t keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool IsKeyBufferEmpty() const noexcept;
	void FlushKeyBuffer() noexcept;

	//char events
	wchar_t ReadChar() noexcept;
	bool IsCharEmpty() const noexcept;
	void FlushCharBuffer() noexcept;
	
	//flush key and char
	void Flush() noexcept;

	//autorepeat
	void EnableAutoRepeat() noexcept;
	void DisableAutoRepeat() noexcept;
	bool IsAutoRepeatEnabled() const noexcept;

private:
	static constexpr uint16_t m_nKeys = 256u;
	std::bitset<m_nKeys> m_KeyStates;				//store 256 bools as bits in 32 bytes
	bool m_autoRepeatEnabled = false;

	static constexpr uint8_t m_BufferSize = 16u;
	std::queue<Event> m_KeyBuffer;
	std::queue<wchar_t> m_CharBuffer;

	void OnKeyPress(uint8_t keycode) noexcept;
	void OnKeyRelease(uint8_t keycode) noexcept;
	void OnGetChar(wchar_t character) noexcept;
	void ClearKeyStates() noexcept;

	template <typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
};