#include "DFSurface.h"

DFSurface::Exception::Exception(int line, const char* file, std::string note) noexcept
	: CException(line, file), m_note(std::move(note))
{
	//
}

const char* DFSurface::Exception::what() const noexcept
{
	std::stringstream sstr;
	sstr << CException::what() << "\n[Note] " << GetNote();
	whatBuffer = sstr.str();
	return whatBuffer.c_str();
}

const char* DFSurface::Exception::GetType() const noexcept
{
	return "D3DMgr_Surface Exception";
}

const std::string& DFSurface::Exception::GetNote() const noexcept
{
	return m_note;
}