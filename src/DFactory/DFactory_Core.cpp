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
	_SInstance.pD3DMgr->SetResolution(_SInstance.pWndMgr->GetWindowSize().first, _SInstance.pWndMgr->GetWindowSize().second);

	// create 'main' and 'aux' render surfaces
	_SInstance.pD3DMgr->CreateRenderSurface("sfcMain", 1.0f);
	_SInstance.pD3DMgr->CreateRenderSurface("sfcBlur", 1.0f);
	_SInstance.pD3DMgr->CreateRenderSurface("sfcDepth", 1.0f);
	_SInstance.pD3DMgr->CreateRenderSurface("sfcMix", 1.0f);
	_SInstance.pD3DMgr->Surface("sfcBlur")->SetShaders("surface/VS_Surface", "surface/PS_Surface_BlurGauss");
	_SInstance.pD3DMgr->Surface("sfcDepth")->SetShaders("surface/VS_Surface", "surface/PS_SurfaceDepth");
	_SInstance.pD3DMgr->Surface("sfcMix")->SetShaders("surface/VS_Surface", "surface/PS_Surface_Mix");

	// init light manager
	_SInstance.LightM = &LightMgr::Get();

	// init model manager
	_SInstance.ModelM = &DFModelMgr::Get();
	_SInstance.ModelM->pD3DMgr = &_SInstance.pWndMgr->D3D();
	_SInstance.ModelM->pMatMgr = _SInstance.MatM;

	// create default camera, activate and bind it
	//_SInstance.CameraAdd("$camNULL");
	_SInstance.CameraAdd("camMain");
	_SInstance.CameraActivate("camMain", true);
	_SInstance.CameraBindVS();

	// init rendering manager
	_SInstance.RenderM = &RenderGraph::Get();
	_SInstance.ModelM->pRenderMgr = _SInstance.RenderM;

	//create and add default material
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};
	DFMatDesc.name = "Mat_Default";
	DFMatDesc.shaders.vertex = "VS_FlatTexture";
	DFMatDesc.shaders.pixel = "PS_FlatTexture";
	DFMatDesc.textures.tex0 = "default/default.dds";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	DFMatDesc.material.matIntensity = 1.0f;
	DFMatDesc.material.spec_metal = 0.0f;
	DFMatDesc.material.pow_roughness = 0.5f;
	DFMatDesc.effects = DF::Pass::Standard;
	_SInstance.MatM->MatAdd(&DFMatDesc);

	//create and add default RTT material
	DFMatDesc = {};
	DFMatDesc.name = "Mat_RTTDefault";
	DFMatDesc.shaders.vertex = "VS_BasicRTT";
	DFMatDesc.shaders.pixel = "PS_FSPP_Blur32";
	_SInstance.MatM->MatAdd(&DFMatDesc);

	// initialize DFactory procedure manager
	_SInstance.proc.Init();

	// load primary ImGui font
	//_SInstance.m_imFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 14.0f);
	_SInstance.m_imFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("res/font1.ttf", 16.0f);

	// disable ImGui INI file
	ImGui::GetIO().IniFilename = NULL;

	return _SInstance;
}

void DFactory::BeginFrame() noexcept
{
	//Process controls
	proc.ProcessCamera();
	proc.ProcessFunctions();
	
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
	// update camera view matrices
	for (const auto& [camName, camera] : m_Cameras)
	{
		camera->SetView();
		camera->SetViewProj();
	}

	// update camera buffer for use in vertex shader slot 1
	CameraUpdateVS();

	// update model matrix and propagate it among meshes through nodes
	for (auto& it : ModelM->m_Models)
	{
		// if model follows camera - recalculate boundaries every frame
		it.pRootNode->XMUpdate(it.GetModelXMTransform());
	}

	// clear rendering queue and generate new jobs
	ModelM->UpdateRenderer();

	// process lights
	LightM->Draw();

	// render frame
	RenderM->RenderFrame();
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
	DF::framesRendered++;
}

void DFactory::ToggleCulling() noexcept
{
	DF::isCullingEnabled = !DF::isCullingEnabled;
}

const bool& DFactory::DevUIMode() const noexcept
{
	return m_showDevUI;
}

void DFactory::ToggleDevUI() noexcept
{
	m_showDevUI = !m_showDevUI;
}
