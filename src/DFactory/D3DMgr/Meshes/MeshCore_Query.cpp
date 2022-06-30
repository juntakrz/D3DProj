#include "MeshCore.h"

void MeshCore::BeginQuery() noexcept
{
	// if 0 frames passed since the last query result - query again
	(m_framesElapsed == 0) ? DF::D3DM->Context()->Begin(m_pQuery.Get()) : void();
}

void MeshCore::EndQuery() noexcept
{
	// should end querying after the same draw call in the same frame as query
	(m_framesElapsed == 0) ? DF::D3DM->Context()->End(m_pQuery.Get()) : void();
}

void MeshCore::GetQueryResult(MeshCore* pMesh) noexcept
{	
	// delay a set number of frames before retrieving a result and reset the counter
	(m_framesElapsed == m_queryDelay) ? m_framesElapsed = 0 : m_framesElapsed++;

	// write result to a defined mesh, if no mesh ptr is provided - write result to itself
	(pMesh == nullptr) ? pMesh = this : 0;

	if (m_framesElapsed == m_queryDelay)
	{
		//while (S_OK != DF::D3DM->Context()->GetData(m_pQuery.Get(), &pMesh->m_QueryResult, sizeof(UINT64), 0))
		while (S_OK != DF::D3DM->Context()->GetData(m_pQuery.Get(), &pMesh->m_queryResult, sizeof(BOOL), 0))
		{
		}
	}
}
