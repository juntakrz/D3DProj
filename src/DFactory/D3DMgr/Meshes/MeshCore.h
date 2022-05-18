#pragma once

#include "../D3DMgr.h"
#include "../../Common/DF_Math.h"
#include "../IBind/Bind_Includes.h"
#include "../../Common/DF_Const.h"
#include "../../DFMaterial.h"

namespace Bind
{
	class VertexBuffer;
}

class MeshCore
{
private:
	template <class T>
	friend class MeshBase;

	virtual const std::vector<std::unique_ptr<Bind::IBind>>& GetStaticBinds() const noexcept = 0;

protected:

	static DFMaterial& MatMgr;

	struct MaterialPSConstBuffer		//material constant buffer for PS
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT3A F0;
		DirectX::XMFLOAT4 data;
	} matCBuffer;

	// DirectX variables
	mutable DirectX::XMMATRIX xmMain;
	mutable DirectX::XMFLOAT3A xmPos;	//for storing transformed 3D position

	struct
	{
		DirectX::XMFLOAT3A scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
	} transform;

	// BUFFERS

	// core index buffer
	Bind::IndexBuffer* m_pIndexBuffer = nullptr;

	// local mesh binds
	std::vector<std::unique_ptr<Bind::IBind>> m_Binds;

public:
	//public mesh variables
	bool isRenderTarget = false;
	std::string m_Name = "";				// mesh name (currently unused)
	uint16_t m_MaterialIndex = 0;			// material index used by this mesh with a standard technique
	uint32_t m_TechniqueIds;				// technique ids in bit format, applied to this mesh

protected:
	void AddMaterialBind(uint16_t matIndex) noexcept;	
	void SetMaterial(uint16_t matIndex) noexcept;

#ifdef _DEBUG
	static CDXGIDebug& GetDXGIDebug(D3DMgr& d3dMgr) noexcept
	{
		return d3dMgr.dxgiDebug;
	}
#endif
	;

public:
	MeshCore()
	{
		xmMain = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat3A(&xmPos, DirectX::FXMVECTOR(xmMain.r[3]));

		// initialize bind pointers vector
		for (uint8_t i = 0; i < Bind::MAXBINDS; i++)
		{
			m_Binds.push_back(nullptr);
		}
	};
	MeshCore(const MeshCore&) = delete;
	~MeshCore() = default;

	// get technique id
	uint32_t GetTechniqueIds() const noexcept;

	// binds and draw calls
	void BindCore() const noexcept;									// bind core mesh buffers
	void BindLocal() const noexcept;								// bind other mesh buffers
	std::vector<std::unique_ptr<Bind::IBind>>* Binds() noexcept;	// get standard mesh binds
	const Bind::IndexBuffer* GetIndexBuffer() const noexcept;	
	void DrawIndexed() noexcept;								// mesh draw call

	//virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept;
	void XMUpdate(FXMMATRIX transform) noexcept;

	DirectX::XMMATRIX* GetMatrix() noexcept;
	const DirectX::XMMATRIX* GetMatrix() const noexcept;
	void SetMatrix(DirectX::FXMMATRIX& matrix) noexcept;

	DirectX::XMFLOAT3A* GetXMPos() noexcept;

	void SetMaterial(std::string name) noexcept;
	void SetMaterialRT(std::string name) noexcept;
	void SetShaders(std::string& inVS, std::string& inPS) noexcept;
	void SetEffects(uint32_t techniqueIds) noexcept;

	//set origin position
	void SetPos(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	DirectX::XMFLOAT3A GetPos() noexcept;

	//translate from origin position
	void TranslateXYZ(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void TranslateX(float x = 0.0f) noexcept;
	void TranslateY(float y = 0.0f) noexcept;
	void TranslateZ(float z = 0.0f) noexcept;

	//set absolute scale values
	void ScaleXYZ(float x = 1.0f, float y = 1.0f, float z = 1.0f) noexcept;
	void ScaleX(float x = 1.0f) noexcept;
	void ScaleY(float y = 1.0f) noexcept;
	void ScaleZ(float z = 1.0f) noexcept;

	//set absolute rotation values
	void RotateXYZ(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void RotateX(float x = 0.0f) noexcept;
	void RotateY(float y = 0.0f) noexcept;
	void RotateZ(float z = 0.0f) noexcept;
};