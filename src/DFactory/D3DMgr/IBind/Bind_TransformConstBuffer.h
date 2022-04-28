#pragma once

#include "../Meshes/MeshCore.h"

namespace Bind
{
	class TransformConstBuffer : public IBind
	{
		struct Transforms
		{
			DirectX::XMMATRIX world;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX viewProj;
		} transforms;

		static std::unique_ptr<ConstVertexBuffer<Transforms>> m_pConstVertexBuffer;
		const MeshCore& parent;
	public:
		TransformConstBuffer(const MeshCore& parent);
		void Bind() noexcept override;
	};
}