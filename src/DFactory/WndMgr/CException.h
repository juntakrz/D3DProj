#pragma once

#include "../../pch.h"

class CException : public std::exception
{
	int m_line = -1;
	const char* m_file = nullptr;

protected:
	mutable std::string whatBuffer;

public:
	CException(int line, const char* file) noexcept;	//exception in 'file' at 'line'

	const char* what() const noexcept override;
	virtual const char* GetType() const noexcept;		//exception type name

	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;		//origin of the exception
};