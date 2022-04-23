#include "MeshImport.h"

MeshImport::MeshImport(D3DMgr* pD3DMgr, std::vector<std::unique_ptr<Bind::IBind>> pBinds)
{
	m_pD3DMgr = pD3DMgr;

	if (!IsStaticBindsInitialized())
	{
		//create and bind topology
		AddStaticBind(std::make_unique<Bind::Topology>(*m_pD3DMgr, D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		//get index buffer for all the following insantces of this object
		SetIndexBufferFromStaticBinds();
	}

	auto indexBuf = static_cast<Bind::IndexBuffer*>(pBinds[Bind::idIndexBuffer].get());
	AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{indexBuf});
	pBinds[Bind::idIndexBuffer].release();

	for (uint8_t id = 1; id < pBinds.size(); id++)
	{
		pBinds[id] != nullptr ? AddBind(std::move(pBinds[id]), id) : void();
	}

	AddBind(std::make_unique<Bind::TransformConstBuffer>(*m_pD3DMgr, *this), Bind::idTransform);
}

XMMATRIX MeshImport::GetTransformXM() const noexcept
{
	DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

	return xmMain;
}
