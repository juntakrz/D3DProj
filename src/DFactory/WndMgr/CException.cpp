#include "CException.h"

CException::CException(int line, const char* file) noexcept : m_line(line), m_file(file)
{
	//
}

const char* CException::what() const noexcept
{
	std::stringstream sstr;
	sstr << GetType() << "\n\n" << GetOriginString();
	whatBuffer = sstr.str();
	return whatBuffer.c_str();
}

const char* CException::GetType() const noexcept
{
	return "Program Exception";
}

int CException::GetLine() const noexcept
{
	return m_line;
}

const std::string& CException::GetFile() const noexcept
{
	return m_file;
}

std::string CException::GetOriginString() const noexcept
{
	std::stringstream sstr;
	sstr << "File: " << m_file << ", line: " << m_line;
	return sstr.str();
}