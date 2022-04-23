#pragma once

#include "D3DMgr/D3DMgr.h"

class DFSurface
{
public:
	class Color
	{
	public:
		uint32_t m_dword;

		constexpr Color() noexcept : m_dword() {};

		constexpr Color(const Color& color) noexcept : m_dword(color.m_dword) {};

		constexpr Color(uint32_t dword) noexcept : m_dword(dword) {};

		constexpr Color(uint8_t x, uint8_t r, uint8_t g, uint8_t b) noexcept
			: m_dword(x << 24u | r << 16u | g << 8u | b) {};

		constexpr Color(uint8_t r, uint8_t g, uint8_t b) noexcept
			: m_dword(r << 16u | g << 16u | b << 8u) {};
		constexpr Color(const Color& color, uint8_t x) noexcept
			: m_dword(x << 24u | color.m_dword) {};
		
		Color& operator=(Color other) noexcept
		{
			m_dword = other.m_dword;
			return *this;
		}

		constexpr uint8_t GetX() const noexcept
		{
			return m_dword >> 24u;
		}
		constexpr uint8_t GetA() const noexcept
		{
			return GetX();
		}
		constexpr uint8_t GetR() const noexcept
		{
			return (m_dword >> 16u) & 0xFFu;
		}
		constexpr uint8_t GetG() const noexcept
		{
			return (m_dword >> 8u) & 0xFFu;
		}
		constexpr uint8_t GetB() const noexcept
		{
			return m_dword & 0xFFu;
		}

		void SetX(const uint8_t x) noexcept
		{
			m_dword = (m_dword & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(const uint8_t a) noexcept
		{
			SetX(a);
		}
		void SetR(const uint8_t r) noexcept
		{
			m_dword = (m_dword & 0xFF00FFFFu) | (r << 16u);

		}
		void SetG(const uint8_t g) noexcept
		{
			m_dword = (m_dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(const uint8_t b) noexcept
		{
			m_dword = (m_dword & 0xFFFFFF00u) | b;
		}
	};

public:
	class Exception : public CException
	{
	public:
		Exception(int line, const char* file, std::string note) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
	private:
		std::string m_note;
	};

public:
	DFSurface() = default;
	DFSurface(uint16_t width, uint16_t height) noexcept;
	DFSurface(DFSurface&& other) noexcept;
	DFSurface(DFSurface&) = delete;
	DFSurface& operator=(DFSurface&& other) noexcept;
	DFSurface& operator=(const DFSurface&) = delete;
	~DFSurface() {};

	void Clear(Color fillColor) noexcept;
	void SetPixel(uint16_t x, uint16_t y, Color color) noexcept;
	Color GetPixel(uint16_t x, uint16_t y) noexcept;
	uint16_t GetWidth() const noexcept;
	uint16_t GetHeight() const noexcept;
	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	static DFSurface LoadFromFile(const std::wstring& filePath) noexcept;
	static DFSurface LoadFromFile(const std::string& filePath) noexcept;		//UTF8/ANSI version
	void SaveToFile(const std::wstring filePath) noexcept;
	void Copy(const DFSurface& other) noexcept;

private:
	DFSurface(uint16_t width, uint16_t height, std::unique_ptr<Color[]> pBuffer) noexcept;

	std::unique_ptr<Color[]> m_pBuffer;
	uint16_t m_width;
	uint16_t m_height;
}; 