#pragma once

#include "../D3DMgr.h"
#include "../Meshes/MeshPointLight.h"
#include "../IBind/Bind_ConstBuffer.h"

class LightMgr
{
private:
	uint16_t m_selPLight = 0;

	// dir light view / proj matrix variables
	DirectX::XMVECTOR m_vecUp = { 0.0f, 1.0f, 0.0f };
	float m_FOV = 1.0f;

	ECamera* m_pDirCamera = nullptr;

	// point light unit
	struct PLight
	{
		std::string name;
		std::unique_ptr<MeshPointLight> pMesh;
		float intensity = 1.0f;
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	// need to make all these variables externally set
	struct DirLight_ConstPSBuffer {
		DirectX::XMFLOAT3 pos = { -2.4f, 1.7f, 0.0f };
		float intensity = 1.0f;
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	} dlData;

	struct DirLight_ConstVSBuffer {
		DirectX::XMMATRIX DLViewProj[(uint8_t)DF::CSM::cascades];
	} dlViewProj;

	struct SPointLight
	{
		DirectX::XMFLOAT3A pos;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT2A intensity;
	};

	struct PointLight_ConstPSBuffer {
		DirectX::XMUINT4 numPLights;
		SPointLight PL[DF::maxPointLights];
	} plData;

	std::vector<PLight> m_PLights;

	mutable Bind::ConstPixelBuffer<DirLight_ConstPSBuffer> dirBuffer;
	mutable Bind::ConstVertexBuffer<DirLight_ConstVSBuffer> dirVSBuffer;
	mutable Bind::ConstPixelBuffer<PointLight_ConstPSBuffer> pointBuffer;

public:
	LightMgr() // initialize buffers with their GPU registers
		: dirBuffer(1u), dirVSBuffer(2u), pointBuffer(2u) {
		m_PLights.reserve(16);
	};
	~LightMgr() = default;
	LightMgr(const LightMgr&) = delete;

	static LightMgr& Get() noexcept {
		static LightMgr _SInstance;
		return _SInstance;
	}

	// DIRECTIONAL LIGHT
	DirLight_ConstPSBuffer& DL() noexcept;

	void DLSetCamera(ECamera* pCam) noexcept;
	void DLVSBufferClear() noexcept;
	void DLVSBufferSetViewProj(uint8_t index) noexcept;
	void DLVSBufferBind() noexcept;

	void DLSetPos(const float& x, const float& y, const float& z) noexcept;
	void DLSetPos(const XMFLOAT3A& pos) noexcept;

	const XMFLOAT3& DLGetPos() const noexcept;
	const XMFLOAT3A& DLGetPosA() const noexcept;

	// POINT LIGHT
	uint16_t GetMaxPLights() const noexcept;

	uint16_t PLAdd(std::string name, float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;

	PLight* PL() noexcept;
	PLight* PL(std::string name) noexcept;
	PLight* PL(uint16_t index) noexcept;

	bool& ShowPLMeshes() noexcept;

	void ShowControls() noexcept;
	void Reset() noexcept;

	void Draw() const noexcept;
	void Bind(const DirectX::XMMATRIX& camView, MeshCore* mesh) noexcept;
};