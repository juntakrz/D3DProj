#include "Bind_TransformConstBuffer.h"
#include "../../DFactory.h"

namespace Bind
{
	std::unique_ptr<ConstVertexBuffer<TransformConstBuffer::Transforms>> TransformConstBuffer::m_pConstVertexBuffer;

	TransformConstBuffer::TransformConstBuffer(D3DMgr& d3dMgr, const MeshCore& parent)
		:
		parent(parent)
	{
		if (!m_pConstVertexBuffer)
		{
			m_pConstVertexBuffer = std::make_unique<ConstVertexBuffer<TransformConstBuffer::Transforms>>(d3dMgr);
		}
	}

	void TransformConstBuffer::Bind(D3DMgr& d3dMgr) noexcept
	{
		transforms.world = DirectX::XMMatrixTranspose(parent.GetTransformXM());
		transforms.view = DirectX::XMMatrixTranspose(d3dMgr.GetCamera()->m_XMView);
		transforms.viewProj = DFactory::Get().m_XMViewProj;

		m_pConstVertexBuffer->Update(d3dMgr, transforms);
		m_pConstVertexBuffer->Bind(d3dMgr);
	}
}