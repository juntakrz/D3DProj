#include "D3DMgr.h"

D3DMgr::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> messages) noexcept
	: Exception(line, file), hr(hr)
{
	//compiling error messages into a single 'errorInfo' string
	m_ErrorInfo += "[DXGIDebug]:\n";

	for (const auto& m : messages)
	{
		m_ErrorInfo += m;
		m_ErrorInfo.push_back('\n');
	}

	/*
	//remove final newline if exists
	if (!m_ErrorInfo.empty())
	{
		m_ErrorInfo.pop_back();
	}*/
}

const char* D3DMgr::HrException::what() const noexcept
{
	std::stringstream sstr;
	std::string dxgiDebugMsg = (!m_ErrorInfo.empty()) ? m_ErrorInfo : "";

	sstr << GetType() << "\n\n" << "Error code: 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (uint32_t)GetErrorCode() << ") " << "\n" << "Error string: "
		<< GetErrorString() << "\n" << "Description: " << GetErrorDesc() << "\n"
#ifdef _DEBUG
		<< dxgiDebugMsg << "\n" << GetOriginString()
#endif
		;
	whatBuffer = sstr.str();
	return whatBuffer.c_str();
}

const char* D3DMgr::HrException::GetType() const noexcept
{
	return "Direct3D Exception";
}

HRESULT D3DMgr::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string D3DMgr::HrException::GetErrorString() const noexcept
{
	return DXGetErrorStringA(hr);
}

std::string D3DMgr::HrException::GetErrorDesc() const noexcept
{
	char buf[384];
	DXGetErrorDescriptionA(hr, buf, sizeof(buf));
	return buf;
}

std::string D3DMgr::HrException::GetErrorInfo() const noexcept
{
	return m_ErrorInfo;
}

D3DMgr::InfoException::InfoException(int line, const char* file, std::vector<std::string> messages) noexcept
	: Exception(line, file)
{
	//compiling error messages into a single 'errorInfo' string
	m_ErrorInfo += "[DXGIDebug]:\n";

	for (const auto& m : messages)
	{
		m_ErrorInfo += m;
		m_ErrorInfo.push_back('\n');
	}
}

const char* D3DMgr::InfoException::what() const noexcept
{
	std::stringstream sstr;
	std::string dxgiDebugMsg = (!m_ErrorInfo.empty()) ? m_ErrorInfo : "";

	sstr << GetType() << "\n\n" << dxgiDebugMsg << "\n" << GetOriginString();
	whatBuffer = sstr.str();
	return whatBuffer.c_str();
}

const char* D3DMgr::InfoException::GetType() const noexcept
{
	return "Direct3D Exception";
}

std::string D3DMgr::InfoException::GetErrorInfo() const noexcept
{
	return m_ErrorInfo;
}

const char* D3DMgr::DeviceRemovedException::GetType() const noexcept
{
	return "Direct3D Device Removed";
}