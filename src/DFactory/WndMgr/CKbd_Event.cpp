#include "../../pch.h"
#include "CKbd.h"

bool CKbd::Event::IsPress() const noexcept
{
	return m_type == Type::Press;
}

bool CKbd::Event::IsRelease() const noexcept
{
	return m_type == Type::Release;
}

bool CKbd::Event::IsValid() const noexcept
{
	return m_type != Type::Invalid;
}

uint8_t CKbd::Event::GetCode() const noexcept
{
	return m_code;
}