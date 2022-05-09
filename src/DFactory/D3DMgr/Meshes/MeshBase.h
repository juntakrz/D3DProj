#pragma once

#include "MeshCore.h"

template <class T>
class MeshBase : public MeshCore
{
	static std::vector<std::unique_ptr<Bind::IBind>> m_StaticBinds;

protected:

	const std::vector<std::unique_ptr<Bind::IBind>>& GetStaticBinds() const noexcept override
	{
		return m_StaticBinds;
	}

	/*
	void SetIndexBufferFromStaticBinds() noexcept
	{
		ASSERT(m_pCIndexBuffer == nullptr);		//check if D3DMgr_IndexBuffer is already bound
		for (const auto& it : m_StaticBinds)
		{
			//if index buffer found among binds - assign it and exit
			if (const auto& target = dynamic_cast<Bind::IndexBuffer*>(it.get()))
			{
				m_pCIndexBuffer = target;
				return;
			}
		}
		ASSERT(m_pCIndexBuffer == nullptr);		//check if index buffer was ever bound

	}*/

};

//defining static variable
template <class T>
std::vector<std::unique_ptr<Bind::IBind>> MeshBase<T>::m_StaticBinds;