#pragma once

#include "../D3DMgr.h"
#include "../../Common/DF_Math.h"
#include "../IBind/IBind.h"
#include "../IBind/Bind_Includes.h"
#include "../../Common/DF_Const.h"
#include "../../DFMaterial.h"

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

	//DirectX variables
	mutable DirectX::XMMATRIX xmMain;
	mutable DirectX::XMFLOAT3A xmPos;	//for storing transformed 3D position

	struct
	{
		DirectX::XMFLOAT3A scale = { 1.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
	} transform;

private:
	std::vector<std::unique_ptr<Bind::IBind>> m_Binds;
	const Bind::IndexBuffer* m_pCIndexBuffer = nullptr;

public:
	bool isRenderTarget = false;

protected:
	void AddBind(std::unique_ptr<Bind::IBind> bindObj, uint8_t id) noexcept;
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> ibuf) noexcept;
	void AddMaterialBind(uint16_t matIndex) noexcept;

	std::vector<std::unique_ptr<Bind::IBind>>* GetBinds() noexcept;
	const Bind::IndexBuffer* GetIndexBuffer() const noexcept;

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
		for (uint8_t i = 0; i < 24; i++)
		{
			m_Binds.push_back(nullptr);
		}
	};
	MeshCore(const MeshCore&) = delete;
	~MeshCore() = default;

	void Draw();

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void XMUpdate(FXMMATRIX transform) noexcept;

	DirectX::XMMATRIX* GetMatrix() noexcept;
	const DirectX::XMMATRIX* GetMatrix() const noexcept;
	void SetMatrix(DirectX::FXMMATRIX& matrix) noexcept;

	DirectX::XMFLOAT3A* GetXMPos() noexcept;

	void SetMaterial(std::string name) noexcept;
	void SetMaterialRT(std::string name) noexcept;
	void SetShaders(std::string& inVS, std::string& inPS) noexcept;

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