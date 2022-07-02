#include "../pch.h"
#include "DFRefMgr.h"

bool DFRefMgr::Add(const std::string& id, void* ptr, Type type) noexcept
{
	if (m_refList.try_emplace(id).second) {

		m_refList.at(id).ptr = ptr;
		m_refList.at(id).type = type;

		return true;
	}

#ifdef _DEBUG || _DFDEBUG
	std::string msg = "RefMgr: couldn't emplace '" + id + "' - already present.\n";
	OutputDebugStringA(msg.c_str());
#endif
	
	return false;
}

std::pair<void*, DFRefMgr::Type> DFRefMgr::Get(const std::string& id) noexcept
{
	if (m_refList.find(id) != m_refList.end()) {
		
		return { m_refList.at(id).ptr, m_refList.at(id).type };
	}

	return { nullptr, DFRefMgr::Type::Null };
}

bool DFRefMgr::Get(const std::string& id, void* out_ptr, uint8_t type) noexcept
{
	if (m_refList.find(id) != m_refList.end()) {

		out_ptr = m_refList.at(id).ptr;
		type = (uint8_t)m_refList.at(id).type;

		return true;
	}

	return false;
}

bool DFRefMgr::Remove(const std::string& id) noexcept
{
	return m_refList.erase(id);
}
