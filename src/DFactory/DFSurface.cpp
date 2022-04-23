#include "DFSurface.h"

DFSurface::DFSurface(uint16_t width, uint16_t height) noexcept
	: m_width(width), m_height(height), m_pBuffer(std::make_unique<Color[]>(width * height))
{
}

DFSurface::DFSurface(DFSurface&& other) noexcept
	: m_pBuffer(std::move(other.m_pBuffer)), m_width(other.m_width), m_height(other.m_height)
{
}

DFSurface& DFSurface::operator=(DFSurface&& other) noexcept
{
	m_width = other.m_width;
	m_height = other.m_height;
	m_pBuffer = std::move(other.m_pBuffer);
	other.m_pBuffer = nullptr;
	return *this;
}

void DFSurface::Clear(Color fillColor) noexcept
{
	memset(m_pBuffer.get(), fillColor.m_dword, m_width * m_height * sizeof(Color));
}

void DFSurface::SetPixel(uint16_t x, uint16_t y, Color color) noexcept
{
	ASSERT(x >= 0 && x < m_width);
	ASSERT(y >= 0 && y < m_height);

	m_pBuffer[y * m_width + x] = color;
}

DFSurface::Color DFSurface::GetPixel(uint16_t x, uint16_t y) noexcept
{
	ASSERT(x >= 0 && x < m_width);
	ASSERT(y >= 0 && y < m_height);

	return m_pBuffer[y * m_width + x];
}

uint16_t DFSurface::GetWidth() const noexcept
{
	return m_width;
}

uint16_t DFSurface::GetHeight() const noexcept
{
	return m_height;
}

DFSurface::Color* DFSurface::GetBufferPtr() noexcept
{
	return m_pBuffer.get();
}

const DFSurface::Color* DFSurface::GetBufferPtr() const noexcept
{
	return m_pBuffer.get();
}

DFSurface DFSurface::LoadFromFile(const std::wstring& filePath) noexcept
{
	uint16_t width = 0, height = 0;
	std::unique_ptr<Color[]> pBuffer;

	{
		Gdiplus::Bitmap bitmap(filePath.c_str());

		if (bitmap.GetLastStatus() != Gdiplus::Status::Ok)
		{
			std::wstringstream wss;
			wss << "Failed to load bitmap from " << filePath.c_str();
			MessageBox(nullptr, std::wstring(wss.str()).c_str(), L"Error", MB_OK | MB_ICONWARNING);
			//throw Exception(__LINE__, __FILE__, wss.str());
		}
		
		width = bitmap.GetWidth();
		height = bitmap.GetHeight();

		pBuffer = std::make_unique<Color[]>(width * height);

		for (uint16_t y = 0; y < height; y++)
		{
			for (uint16_t x = 0; x < width; x++)
			{
				Gdiplus::Color color;
				bitmap.GetPixel(x, y, &color);
				pBuffer[y * width + x] = color.GetValue();
			}
		}
	}

	return DFSurface(width, height, std::move(pBuffer) );
}

DFSurface DFSurface::LoadFromFile(const std::string& filePath) noexcept
{
	std::wstring wPath(filePath.begin(), filePath.end());
	return DFSurface::LoadFromFile(wPath);
}

void DFSurface::SaveToFile(const std::wstring filePath) noexcept
{
	auto GetEncoderClsID = [&filePath](const WCHAR* format, CLSID* pClsID) -> void
	{
		UINT num = 0;	//number of image encoders
		UINT size = 0;	//size of encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = nullptr;

		Gdiplus::GetImageEncodersSize(&num, &size);

		if (size == 0)
		{
			std::wstringstream wss;
			wss << "Saving surface to " << filePath << " failed. Encoder size is 0.";
			//throw Exception(__LINE__, __FILE__, wss.str());
		}

		//allocating memory
		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == nullptr)
		{
			std::wstringstream wss;
			wss << "Saving surface to " << filePath << " failed. Couldn't allocate memory.";
			//throw Exception(__LINE__, __FILE__, wss.str());
		}

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT i = 0; i < num; i++)
		{
			if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0)
			{
				*pClsID = pImageCodecInfo[i].Clsid;
				free(pImageCodecInfo);
				return;
			}
		}

		free(pImageCodecInfo);

		std::wstringstream wss;
		wss << "Saving surface to " << filePath <<
			" failed. Couldn't find matching encoder.";
		//throw Exception(__LINE__, __FILE__, wss.str());
	};

	CLSID bmpID;
	GetEncoderClsID(L"image/bmp", &bmpID);

	Gdiplus::Bitmap bitmap(m_width, m_height, m_width * sizeof(Color), PixelFormat32bppARGB, (BYTE*)m_pBuffer.get());
	if (bitmap.Save(filePath.c_str(), &bmpID, nullptr) != Gdiplus::Status::Ok)
	{
		std::wstringstream wss;
		wss << "Failed to save the surface to " << filePath;
		//throw Exception(__LINE__, __FILE__, wss.str());
	}
}

void DFSurface::Copy(const DFSurface& other) noexcept
{
	ASSERT(m_width == other.m_width);
	ASSERT(m_height == other.m_height);
	memcpy(m_pBuffer.get(), other.m_pBuffer.get(), m_width * m_height * sizeof(Color));
}

DFSurface::DFSurface(uint16_t width, uint16_t height, std::unique_ptr<Color[]> pBuffer) noexcept
	: m_width(width), m_height(height), m_pBuffer(std::move(pBuffer))
{
}
