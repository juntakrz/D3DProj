#include "DFactory.h"

DFactory DFactory::_SInstance;

DFactory& DFactory::Init(DFACTORY_INIT_DESC* pDescription)
{
	// pointer to engine manager (this)
	DF::Engine = &_SInstance;
	
	// initialize WinAPI manager and create window
	_SInstance.pWndMgr = std::make_unique<WndMgr>(pDescription->Wnd.sizeX, pDescription->Wnd.sizeY, pDescription->Wnd.posX, pDescription->Wnd.posY,
		pDescription->WndTitle.c_str());

	// retrieve D3DMgr
	DF::D3DM = &_SInstance.pWndMgr->D3D();
	_SInstance.pD3DMgr = DF::D3DM;

	// set Direct3D default viewport size
	_SInstance.pD3DMgr->SetViewportSize(_SInstance.pWndMgr->GetWindowSize().first, _SInstance.pWndMgr->GetWindowSize().second);

	// create 'main' and 'aux' render surfaces
	_SInstance.pD3DMgr->CreateRenderSurface("main", 1.0f);
	_SInstance.pD3DMgr->CreateRenderSurface("aux", 1.0f);
	_SInstance.pD3DMgr->RenderSurfaces(1)->SetShaders("surface//VS_Surface", "surface//PS_Surface_Blur8");

	// init light manager
	_SInstance.LightM = &LightMgr::Get();

	// init model manager
	_SInstance.ModelM = &DFModelMgr::Get();
	_SInstance.ModelM->pD3DMgr = &_SInstance.pWndMgr->D3D();
	_SInstance.ModelM->pMatMgr = _SInstance.MatM;

	// create default camera, activate and bind it
	_SInstance.CameraAdd("camMain");
	_SInstance.CameraActivate("camMain", true);
	_SInstance.CameraBindVS();

	// init rendering manager
	_SInstance.RenderM = new RenderQ;
	_SInstance.ModelM->pRenderMgr = _SInstance.RenderM;

	//create and add default material
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};
	DFMatDesc.name = "Mat_Default";
	DFMatDesc.shaders.vertex = "VS_BasicTexture";
	DFMatDesc.shaders.pixel = "PS_BasicTexture";
	DFMatDesc.textures.tex0 = "default//default.dds";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	DFMatDesc.material.matIntensity = 1.0f;
	DFMatDesc.material.spec_metal = 0.0f;
	DFMatDesc.material.pow_roughness = 0.5f;
	_SInstance.MatM->MatAdd(&DFMatDesc);

	//create and add default RTT material
	DFMatDesc = {};
	DFMatDesc.name = "Mat_RTTDefault";
	DFMatDesc.shaders.vertex = "VS_BasicRTT";
	DFMatDesc.shaders.pixel = "PS_FSPP_Blur32";
	_SInstance.MatM->MatAdd(&DFMatDesc);

	// initialize DFactory procedure manager
	_SInstance.proc.Init();

	return _SInstance;
}

void DFactory::BeginFrame() noexcept
{
	//Process controls
	proc.ProcessCamera();
	proc.ProcessFunctions();

	//calculate View and ViewProj matrices only once per frame
	pD3DMgr->Camera()->SetView();
	m_XMViewProj = XMMatrixTranspose(XMMatrixMultiply(pD3DMgr->Camera()->m_XMView, m_XMProj));

	//hand off to Direct3D manager
	pD3DMgr->BeginFrame();
}

void DFactory::EndFrame() noexcept
{
	pD3DMgr->EndFrame();

	// increase frames rendered count by one
	FrameCountIncrease();
}

void DFactory::DrawFrame() noexcept
{
	// update camera buffer for use in vertex shader slot 1
	CameraUpdateVS();

	// update directional light view
	LightM->DLSetViewMatrix();

	// update model matrix and propagate it among meshes through nodes
	for (auto& it : ModelM->m_Models)
	{
		it.pRootNode->XMUpdate(it.GetModelXMTransform());
	}
	
	// process lights
	LightM->Draw();

	// render frame
	RenderM->Render();
}

void DFactory::UpdateRenderer() noexcept
{
	ModelM->UpdateRenderer();
}

void DFactory::SetSimulationSpeed(const float simSpeedFactor) noexcept
{
	vars.simSpeedFactor = simSpeedFactor;
}

const float& DFactory::GetSimulationSpeed() const noexcept
{
	return vars.simSpeedFactor;
}

void DFactory::FrameCountIncrease() noexcept
{
	m_frames++;
}

const uint64_t& DFactory::GetFrameCount() const noexcept
{
	return m_frames;
}

const XMMATRIX* DFactory::GetProjection() const noexcept
{
	return &m_XMProj;
}

void DFactory::SetProjection(float FOV, float ratio, float nearZ, float farZ) noexcept
{
	m_XMProj = XMMatrixPerspectiveFovLH(FOV, ratio, nearZ, farZ);
}
