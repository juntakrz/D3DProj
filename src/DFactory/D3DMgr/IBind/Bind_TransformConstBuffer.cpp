#include "Bind_TransformConstBuffer.h"
#include "../../DFactory.h"

namespace Bind
{
	std::unique_ptr<ConstVertexBuffer<TransformConstBuffer::Transforms>> TransformConstBuffer::m_pConstVertexBuffer;

	TransformConstBuffer::TransformConstBuffer(const MeshCore& parent)
		:
		parent(parent)
	{
		if (!m_pConstVertexBuffer)
		{
			m_pConstVertexBuffer = std::make_unique<ConstVertexBuffer<TransformConstBuffer::Transforms>>();
		}
	}

	void TransformConstBuffer::Bind() noexcept
	{
		transforms.world = DirectX::XMMatrixTranspose(parent.GetTransformXM());
		transforms.view = DirectX::XMMatrixTranspose(DF::D3DM->Camera()->m_XMView);
		transforms.viewProj = DFactory::Get().m_XMViewProj;

		m_pConstVertexBuffer->Update(transforms);
		m_pConstVertexBuffer->Bind();
	}
}