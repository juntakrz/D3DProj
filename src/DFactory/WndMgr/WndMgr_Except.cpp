#include "../../pch.h"
#include "WndMgr.h"

WndMgr::Exception::Exception(int line, const char* file, HRESULT hr) noexcept : CException(line, file), m_hr(hr)
{
	//
}

const char* WndMgr::Exception::what() const noexcept
{
	std::stringstream sstr;
	sstr << GetType() << "\nError code 0x" << GetErrorCode() << ": " << GetErrorString() << "\n"
#ifdef _DEBUG
		<< GetOriginString()
#endif
		;
	whatBuffer = sstr.str();
	return whatBuffer.c_str();
}

const char* WndMgr::Exception::GetType() const noexcept
{
	return "WinAPI Exception";
}

std::string WndMgr::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD msgLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr);
	if (!msgLength)
	{
		return "Unindentified error code!\n";
	}
	std::string errorStr = pMsgBuf;
	LocalFree(pMsgBuf); //frees memory that is LocalAlloc by FormatMessage
	return errorStr;
}

HRESULT WndMgr::Exception::GetErrorCode() const noexcept
{
	return m_hr;
}

std::string WndMgr::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(m_hr);
}