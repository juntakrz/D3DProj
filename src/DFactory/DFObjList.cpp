#include "../pch.h"
#include "DFObjList.h"

void DFObjList::Add(const std::string& id, void* ptr, Type type)
{
}

std::pair<void*, DFObjList::Type> DFObjList::Get(const std::string& id)
{
	if (m_objList.find(id) != m_objList.end()) {
		
		return { m_objList.at(id).ptr, m_objList.at(id).type };
	}

	return { nullptr, DFObjList::Type::Null };
}

void DFObjList::Remove(const std::string& id)
{
}
