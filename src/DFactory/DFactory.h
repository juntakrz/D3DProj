#pragma once

#include "D3DMgr/Meshes/Mesh_Includes.h"
#include "D3DMgr/Lights/LightMgr.h"
#include "D3DMgr/Cameras/CCamera.h"
#include "WndMgr/WndMgr.h"
#include "Common/DF_A.h"
#include "DFMaterial.h"
#include "DFModelMgr.h"
#include "RenderQ/RenderQ.h"

class DFactory
{
	struct Mesh
	{
		std::string name;
		std::unique_ptr<MeshCore> pMesh;
	};

	struct Camera
	{
		std::string name;
		std::unique_ptr<CCamera> pCam;
	};

	struct CameraConstVSBuffer
	{
		XMFLOAT3A pos = { 0.0f, 0.0f, 0.0f };
	} cameraConstBuffer;

	struct
	{
		float simSpeedFactor = 1.0f;
		uint16_t activeCamera = 0;
		uint16_t selectedCamera = 0;
		uint16_t selectedLight = 0;
	} vars;

	struct
	{
		int64_t int64bit = 0;
		float64 float64bit = 0.0f;
	} debug;

	class CProc
	{
		// managers
		DFactory* pDF;
		WndMgr* pWnd;
		CKbd* pKbd;
		CMouse* pMouse;
		
		// view variables
		POINT m_cursorPos;
		int16_t m_oldPosX = 0, m_oldPosY = 0;
		DirectX::XMFLOAT4 m_camDir = { 0.0f, 0.0f, 0.0f, 0.0f };	// camera direction
		DirectX::XMFLOAT4 m_camRot = { 0.0f, 0.0f, 0.0f, 0.0f };	// camera rotation

		// state variables
		bool m_RenderWireframe = false;

	public:
		CProc() = delete;
		CProc(DFactory* parent) : pDF(parent) {};

		void Init() noexcept;
		void ProcessCamera() noexcept;
		void ProcessFunctions() noexcept;
	};

	CProc proc;

public:
	struct DFACTORY_INIT_DESC
	{
		std::wstring WndTitle = CWND_WNDNAME;
		bool FullScreen = false;
		
		struct
		{
			uint16_t sizeX = 0, sizeY = 0, posX = 0, posY = 0;
		} Wnd;
	};

	//managers
	std::unique_ptr<WndMgr> pWndMgr;
	D3DMgr* pD3DMgr = nullptr;
	RenderQ* RenderM = nullptr;
	DFModelMgr* ModelM = nullptr;
	LightMgr* LightM = nullptr;
	DFMaterial* MatM = &DFMaterial::Get();
	imGuiMgr& imGUIMgr = imGuiMgr::Get();

	XMMATRIX m_XMViewProj;

private:
	static DFactory _SInstance;

	std::vector<Mesh> m_Meshes;
	std::vector<Camera> m_Cameras;

	std::unique_ptr<Bind::ConstVertexBuffer<CameraConstVSBuffer>> pCamCBuf;

	static constexpr float m_projRatio = CWND_DEFAULTWIDTH / CWND_DEFAULTHEIGHT;
	XMMATRIX m_XMProj = XMMatrixPerspectiveFovLH(1.0f, (float)CWND_DEFAULTWIDTH / (float)CWND_DEFAULTHEIGHT, 0.1f, 5000.0f);

	// frame counter
	uint64_t m_frames = 0u;

public:

	DFactory() : proc(this) {};
	DFactory(const DFactory&) = delete;
	DFactory& operator=(const DFactory&) = delete;
	~DFactory() {};

	static DFactory& Get() {
		return _SInstance;
	}

	static DFactory& Init(DFACTORY_INIT_DESC* pDescription);

	// CORE --------------------------

	void BeginFrame() noexcept;
	void EndFrame() noexcept;
	void DrawFrame() noexcept;

	void UpdateRenderer() noexcept;		// resets the render queue

	void SetSimulationSpeed(const float simSpeedFactor) noexcept;
	const float& GetSimulationSpeed() const noexcept;

	// frame counter
	void AddFrame() noexcept;
	const uint64_t& GetFrame() const noexcept;

	const XMMATRIX* GetProjection() const noexcept;
	void SetProjection(float FOV, float ratio, float nearZ, float farZ) noexcept;

	// CAMERA ------------------------

	//create and add new camera
	void AddCamera(std::string name) noexcept;
	void AddCamera(std::string name, DirectX::XMFLOAT3& pos) noexcept;
	void AddCamera(std::string name = "Camera1", float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;

	//select and set an active camera
	void SetActiveCamera(std::string name, bool select = false) noexcept;
	void SetActiveCamera(uint16_t index = 0, bool select = false) noexcept;

	//select camera
	void SelectCamera(std::string name) noexcept;
	void SelectCamera(uint16_t index = 0) noexcept;

	void CameraBindVS() noexcept;
	void CameraUpdateVS() noexcept;

	//operations with the currently selected camera
	CCamera* Camera() noexcept;
};