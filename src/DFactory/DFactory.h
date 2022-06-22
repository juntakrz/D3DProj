#pragma once

#include "D3DMgr/Lights/LightMgr.h"
#include "WndMgr/WndMgr.h"
#include "DFMaterial.h"
#include "DFModelMgr.h"
#include "RenderGraph/RenderGraph.h"

class DFactory
{
	friend class RenderGraph;	// grant access to vars

	struct
	{
		float simSpeedFactor = 1.0f;
		uint16_t selectedLight = 0;				// selected light index
		std::string activeCamera = "";			// keys for unordered map of cameras
		std::string selectedCamera = "";
	} vars;

	struct CameraConstVSBuffer
	{
		XMFLOAT3A pos = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3A lookAt = { 0.0f, 0.0f, 0.1f };
	} cameraConstBuffer;

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

	struct
	{
		int64_t int64bit = 0;
		uint64_t uint64 = 0;
		double float64bit = 0.0f;
	} debug;

	ImFont* m_imFont = nullptr;

	//managers
	std::unique_ptr<WndMgr> pWndMgr;
	D3DMgr* pD3DMgr = nullptr;
	RenderGraph* RenderM = nullptr;
	DFModelMgr* ModelM = nullptr;
	LightMgr* LightM = nullptr;
	DFMaterial* MatM = &DFMaterial::Get();
	imGuiMgr& imGUIMgr = imGuiMgr::Get();

private:
	static DFactory _SInstance;

	std::unordered_map<std::string, std::unique_ptr<CCamera>> m_Cameras;
	std::unique_ptr<Bind::ConstVertexBuffer<CameraConstVSBuffer>> pCamCBuf;

	bool m_showDevUI = false;

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
	void FrameCountIncrease() noexcept;
	const uint64_t& GetFrameCount() const noexcept;

	// enable or disable culling
	void ToggleCulling() noexcept;
	
	// developer UI methods
	const bool& DevUIMode() const noexcept;
	void ToggleDevUI() noexcept;

	// CAMERA ------------------------

	// create and add new camera
	void CameraAdd(std::string name, float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void CameraAdd(std::string name, DirectX::XMFLOAT3& pos) noexcept;

	// select and set an active camera
	void CameraActivate(std::string name, bool select = false) noexcept;

	// select camera
	void CameraSelect(std::string name) noexcept;

	void CameraBindVS() noexcept;
	void CameraUpdateVS() noexcept;

	// operations with the currently selected camera / retrieve camera without selecting it
	CCamera* Camera(const std::string& name = "") noexcept;

	// get active camera
	std::pair<std::string, CCamera*> CameraGetActive() noexcept;

	// imGui methods
	void ShowEditModelWindow() noexcept;
	void ShowCameraWindow() noexcept;
	void ShowStatsWindow() noexcept;
};