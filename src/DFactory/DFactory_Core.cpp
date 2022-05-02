#include "DFactory.h"

DFactory DFactory::_SInstance;

DFactory& DFactory::Init(DFACTORY_INIT_DESC* pDescription)
{
	// initialize WinAPI manager and create window
	_SInstance.pWndMgr = std::make_unique<WndMgr>(pDescription->Wnd.sizeX, pDescription->Wnd.sizeY, pDescription->Wnd.posX, pDescription->Wnd.posY,
		pDescription->WndTitle.c_str());

	// retrieve D3DMgr
	DF::pD3DM = &_SInstance.pWndMgr->D3D();
	_SInstance.pD3DMgr = DF::pD3DM;

	// set Direct3D default viewport size
	_SInstance.pD3DMgr->SetViewportSize(_SInstance.pWndMgr->GetWindowSize().first, _SInstance.pWndMgr->GetWindowSize().second);

	// init light manager
	_SInstance.LightM = &LightMgr::Get();

	// init model manager
	_SInstance.ModelM = &DFModelMgr::Get();
	_SInstance.ModelM->pD3DMgr = &_SInstance.pWndMgr->D3D();
	_SInstance.ModelM->pMatMgr = _SInstance.MatM;

	// create default objects
	_SInstance.AddCamera();
	_SInstance.CameraBindVS();

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
	DFMatDesc.shaders.pixel = "PS_BasicRTT";
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
	Camera()->SetView();
	m_XMViewProj = XMMatrixTranspose(XMMatrixMultiply(Camera()->m_XMView, m_XMProj));

	//hand off to Direct3D manager
	pD3DMgr->BeginFrame();
}

void DFactory::EndFrame() noexcept
{
	pD3DMgr->EndFrame();
}

void DFactory::DrawFrame() noexcept
{
	//send active camera matrix to current D3DMgr object
	pD3DMgr->SetCamera(m_Cameras[vars.activeCamera].pCam.get());

	//update camera buffer for use in vertex shader slot 1
	CameraUpdateVS();

	//DF::pD3DM->RTSetMain();

	for (auto& it : ModelM->m_Models)
	{
		// update model matrix and propagate it among meshes through nodes
		it.pRootNode->XMUpdate(it.GetModelXMTransform());

		// determine which light sources will affect a certain mesh
		for (auto& mesh : it.meshes)
		{
			LightM->Bind(Camera()->m_XMView, mesh.pMesh.get());
		}

		// draw the model
		it.pRootNode->Draw();

	}
	
	LightM->Draw();
}

void DFactory::SetSimulationSpeed(const float simSpeedFactor) noexcept
{
	vars.simSpeedFactor = simSpeedFactor;
}

const float& DFactory::GetSimulationSpeed() const noexcept
{
	return vars.simSpeedFactor;
}

const XMMATRIX* DFactory::GetProjection() const noexcept
{
	return &m_XMProj;
}

void DFactory::SetProjection(float FOV, float ratio, float nearZ, float farZ) noexcept
{
	m_XMProj = XMMatrixPerspectiveFovLH(FOV, ratio, nearZ, farZ);
}
