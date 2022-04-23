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

	static bool IsStaticBindsInitialized() noexcept
	{
		return !m_StaticBinds.empty();
	}

	static void AddStaticBind(std::unique_ptr<Bind::IBind> pBindObj) noexcept
	{
		ASSERT(typeid(*pBindObj) != typeid(Bind::IndexBuffer));
		m_StaticBinds.push_back(std::move(pBindObj));
	}
	
	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> pIndexBuffer) noexcept
	{
		ASSERT(m_pCIndexBuffer == nullptr);
		m_pCIndexBuffer = pIndexBuffer.get();
		m_StaticBinds.push_back(std::move(pIndexBuffer));
	}

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

	}

};

//defining static variable
template <class T>
std::vector<std::unique_ptr<Bind::IBind>> MeshBase<T>::m_StaticBinds;