#pragma once

#include "../D3DMgr.h"
#include "../Meshes/MeshPointLight.h"
#include "../IBind/Bind_ConstBuffer.h"

class LightMgr
{
private:
	static const uint16_t m_maxPLights = 4;
	uint16_t m_selPLight = 0;

	struct PLight
	{
		std::string name;
		
		std::unique_ptr<MeshPointLight> pMesh;

		float intensity = 1.0f;
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	};

	struct DirLight_ConstPSBuffer {
		DirectX::XMFLOAT3 pos = { -2.4f, 1.7f, 0.0f };
		float intensity = 1.0f;
		DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	} dlData;

	struct PointLight_ConstPSBuffer {
		DirectX::XMFLOAT3A pos[m_maxPLights];
		DirectX::XMFLOAT4 diffuse[m_maxPLights];
		DirectX::XMFLOAT2A intensity[m_maxPLights];
		DirectX::XMUINT4 numPLights;
	} plData;

	std::vector<PLight> m_PLights;

	mutable Bind::ConstPixelBuffer<DirLight_ConstPSBuffer> dirBuffer;
	mutable Bind::ConstPixelBuffer<PointLight_ConstPSBuffer> pointBuffer;

public:
	LightMgr() 
		: dirBuffer(1u), pointBuffer(2u) {
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

	// POINT LIGHT
	uint16_t GetMaxPLights() const noexcept;

	uint16_t PLAdd(std::string name, float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;

	PLight& PL() noexcept;
	PLight& PL(std::string name) noexcept;
	PLight& PL(uint16_t index) noexcept;

	bool& ShowPLMeshes() noexcept;

	void ShowControls() noexcept;
	void Reset() noexcept;

	void Draw() const noexcept;
	void Bind(const DirectX::XMMATRIX& camView, MeshCore* mesh) noexcept;
};