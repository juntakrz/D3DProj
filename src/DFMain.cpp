#include "pch.h"
#include "DFMain.h"

//PRIVATE

DFMain::DFMain(DFactory::DFACTORY_INIT_DESC* pDesc) : DF(DFactory::Init(pDesc))
{
}

void DFMain::DrawFrame()
{
	//deltaA += 0.002f;

	//DF.ModelM->Select("MdlTachi");
	//DF.ModelM->SetRotation(-0.6f - deltaA*0.7f , -0.4f + deltaA, -0.3f + deltaA * 0.7f);

	//DF.LightM->PL(0).pMesh->DEBUG_Rotate(0.01f);
	//DF.LightM->PL(1).pMesh->DEBUG_Rotate(-0.01f);

	DF.BeginFrame();

	DF.DrawFrame();
	
	if (DF.DevUIMode())
	{
		//DF.ShowEditModelWindow();
		DF.ShowCameraWindow();
		DF.ShowStatsWindow();
	}

	DF.EndFrame();

	//DF.ModelM->SetMaterial("Mat_Mars");

	//DF.Light()->ShowControls();
	/*
	if (pD3DMgr->IsImGuiEnabled())
	{
		DF.imGUIMgr.ImGuiEndFrame();
	}
	pD3DMgr->EndFrame();
	*/
}

//PUBLIC

int32_t DFMain::Run()
{
	// 1. Load map
	LoadMap(DF::Default::map);

	// 2. Run message and rendering loops

	MSG msg;
	while (true)
	{
		if (const auto result = WndMgr::PeekMessages(msg))
		{
			return *result;
		}
		DrawFrame();
	}
}

void DFMain::LoadMap(const std::wstring& map) noexcept
{
	// initial setup
	CScriptMgr& SM = CScriptMgr::Get();
	nlohmann::json jsonData;
	std::thread t1;

	const std::wstring mapPath = L"maps/" + map + L".dmap/";		// path to map
	const std::wstring initPath = mapPath + L"init.json";			// load screen data
	const std::wstring camPath = mapPath + L"cameras.json";			// camera objects
	const std::wstring matPath = mapPath + L"materials.json";		// materials to pre-load
	const std::wstring lightPath = mapPath + L"lights.json";		// lights setup and data
	const std::wstring objPath = mapPath + L"objects.json";			// meshes and models to create/pre-load
	const std::wstring commPath = mapPath + L"commands.json";		// post-init commands using previously created objects

	// loading "load screen" data
	SM.JSONLoad(initPath.c_str(), &jsonData);

	// parse loaded JSON file and get materials data
	SM.JSONParseMaterials(jsonData.at("materials"));

	// get load screen material name
	std::string loadScreenMat = jsonData.at("loadScreen").at("useMaterial").get<std::string>();

	SM.JSONLoad(matPath.c_str(), &jsonData);
	t1 = std::thread(&CScriptMgr::JSONParseMaterials, &SM, jsonData.at("materials"));

	// render loading screen
	DF.BeginFrame();

	DF.pD3DMgr->RTBind("rtBack", "dsBack");
	DF.MatM->BindTextureToPS(DF.MatM->Mat(loadScreenMat.c_str()).idTex[0], 0u);		// texture from pre-created load screen material
	DF.pD3DMgr->RenderSurface("sfcMain");

	DF.EndFrame();

	// delete load screen material
	DF.MatM->MatDelete(loadScreenMat.c_str());

	// load map materials
	//SM.JSONLoad(matPath.c_str(), &jsonData);
	//SM.JSONParseMaterials(jsonData.at("materials"));

	// load map cameras
	SM.JSONLoad(camPath.c_str(), &jsonData);
	SM.JSONParseCameras(jsonData);

	// load map lights
	SM.JSONLoad(lightPath.c_str(), &jsonData);
	SM.JSONParseLights(jsonData.at("lights"));

	t1.join();

	SM.JSONLoad(objPath.c_str(), &jsonData);
	SM.JSONParseObjects(jsonData.at("objects"));

	SM.JSONLoad(commPath.c_str(), &jsonData);
	SM.JSONParseCommands(jsonData.at("commands"));
}

void DFMain::LoadMap() noexcept
{
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};

	//Mat_LoadScr
	DFMatDesc.name = "Mat_LoadScr";
	DFMatDesc.shaders.vertex = "VS_FlatTexture";
	DFMatDesc.shaders.pixel = "PS_FlatTexture";
	DFMatDesc.textures.tex0 = "img_loadscr.dds";
	DFMatDesc.manageTextures = true;

	DF.MatM->MatAdd(&DFMatDesc);

	DF::isLoadScreen = true;

	DF.BeginFrame();
	
	DF.ModelM->Create(DF::idPlane, "MdlLoadScr", false);
	DF.ModelM->SetMaterial("Mat_LoadScr");
	DF.ModelM->SetScaling(16.0f, 9.0f, 0.0f);
	DF.ModelM->SetPos(0.0f, 0.0f, 16.5f);

	//DF.UpdateRenderer();
	
	DF.DrawFrame();

	DF.EndFrame();

	DF::isLoadScreen = false;
	
	//Mat_Mars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Mars";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS_NoHDR";
	DFMatDesc.textures.tex0 = "PBR/mars/mars_4k_albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/mars/mars_4k_normal.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.06f, 0.0f, 0.0f };
	DFMatDesc.material.intensity = 1.0f;
	DFMatDesc.material.metalness = 0.0f;
	DFMatDesc.material.roughness = 0.0f;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_MarsPBS
	DFMatDesc = {};
	DFMatDesc.name = "Mat_MarsPBS";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS_NoHDR_L";
	DFMatDesc.textures.tex0 = "PBR/mars/mars_4k_albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/mars/mars_4k_normal.dds";
	DFMatDesc.textures.tex2 = "PBR/mars/mars_4k_metallic.dds";
	DFMatDesc.textures.tex3 = "PBR/mars/mars_4k_roughness.dds";
	DFMatDesc.textures.tex4 = "PBR/mars/mars_4k_ao.dds";
	DFMatDesc.material.ambientColor = { 0.06f, 0.03f, 0.0f, 1.0f };
	DFMatDesc.material.metalness = 0.08f;
	DFMatDesc.material.roughness = 0.8f;
	DFMatDesc.material.F0 = { 0.25f, 0.23f, 0.00f };
	DFMatDesc.material.intensity = 2.2f;
	DFMatDesc.material.bumpiness = 1.5f;
	DFMatDesc.passes = DF::Pass::Background;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_Atmo
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Atmo";
	DFMatDesc.shaders.vertex = "VS_Atmosphere";
	DFMatDesc.shaders.pixel = "PS_Atmosphere";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.15f, 1.0f };
	DFMatDesc.material.metalness = 0.0f;
	DFMatDesc.material.roughness = 1.0f;
	DFMatDesc.material.intensity = 1.0f;
	DFMatDesc.passes = DF::Pass::Blur;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//Mat_Stars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Stars";
	DFMatDesc.shaders.vertex = "VS_FlatTexture";
	DFMatDesc.shaders.pixel = "PS_FlatTexture";
	DFMatDesc.textures.tex0 = "stars_6k_color.dds";
	DFMatDesc.material.intensity = 0.1f;
	DFMatDesc.passes = DF::Pass::Background;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSMetal
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSMetal";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR/iron/rustediron2_albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/iron/rustediron2_normal.dds";
	DFMatDesc.textures.tex2 = "PBR/iron/rustediron2_metallic.dds";
	DFMatDesc.textures.tex3 = "PBR/iron/rustediron2_roughness.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.metalness = 1.0f;
	DFMatDesc.material.roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.0f, 0.0f, 0.0f };
	DFMatDesc.material.bumpiness = 2.0f;
	DFMatDesc.passes = DF::Pass::Standard | DF::Pass::Shadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSGold
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSGold";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR/gold_ornate/albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/gold_ornate/normal.dds";
	DFMatDesc.textures.tex2 = "PBR/gold_ornate/metallic.dds";
	DFMatDesc.textures.tex3 = "PBR/gold_ornate/roughness.dds";
	DFMatDesc.textures.tex4 = "PBR/gold_ornate/ao.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.metalness = 1.0f;
	DFMatDesc.material.roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpiness = 2.0f;
	DFMatDesc.passes = DF::Pass::Standard | DF::Pass::Shadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSSteel
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSSteel";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR/steel_plate/albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/steel_plate/normal.dds";
	DFMatDesc.textures.tex2 = "PBR/steel_plate/metallic.dds";
	DFMatDesc.textures.tex3 = "PBR/steel_plate/roughness.dds";
	DFMatDesc.textures.tex4 = "PBR/steel_plate/ao.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.metalness = 1.0f;
	DFMatDesc.material.roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpiness = 2.0f;
	DFMatDesc.passes = DF::Pass::Blur | DF::Pass::Shadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSBricks
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSBricks";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR/bricks/albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/bricks/normal.dds";
	DFMatDesc.textures.tex3 = "PBR/bricks/roughness.dds";
	DFMatDesc.textures.tex4 = "PBR/bricks/ao.dds";
	DFMatDesc.material.intensity = 5.0f;
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.25f, 1.0f };
	DFMatDesc.material.metalness = 2.0f;
	DFMatDesc.material.roughness = 3.0f;
	DFMatDesc.material.F0 = { 0.07f, 0.06f, 0.05f };
	DFMatDesc.material.bumpiness = 2.2f;
	DFMatDesc.passes = DF::Pass::Standard | DF::Pass::Shadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_Test
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Test";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR/gold_ornate/albedo.dds";
	DFMatDesc.textures.tex1 = "PBR/gold_ornate/normal.dds";
	DFMatDesc.textures.tex2 = "PBR/gold_ornate/metallic.dds";
	DFMatDesc.textures.tex3 = "PBR/gold_ornate/roughness.dds";
	DFMatDesc.textures.tex4 = "PBR/gold_ornate/ao.dds";
	DFMatDesc.material.ambientColor = { 0.2f, 0.2f, 0.02f, 1.0f };
	DFMatDesc.material.metalness = 1.0f;
	DFMatDesc.material.roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpiness = 2.0f;
	DFMatDesc.passes = DF::Pass::Standard | DF::Pass::Shadow;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//Mat_Sun
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Sun";
	DFMatDesc.shaders.vertex = "VS_Default";
	DFMatDesc.shaders.pixel = "PS_Default";
	DFMatDesc.passes = DF::Pass::Blur;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_SunP
	DFMatDesc = {};
	DFMatDesc.name = "Mat_SunP";
	DFMatDesc.shaders.vertex = "VS_QuadFlare";
	DFMatDesc.shaders.geometry = "GS_QuadFlare";
	DFMatDesc.shaders.pixel = "PS_QuadFlare";
	DFMatDesc.textures.tex0 = "Lens/sunFlare.dds";
	DFMatDesc.textures.tex1 = "Lens/lensDust.dds";		// make it a separate command maybe
	DFMatDesc.passes = DF::Pass::PointSprites;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//MdlStars
	DF.ModelM->Create(DF::idSkySphere, "MdlStars");
	DF.ModelM->SetMaterial("Mat_Stars");
	DF.ModelM->SetScaling(400.0f, 400.0f, 400.0f);
	DF.ModelM->SetRotation(0.25f, 2.2f, -0.5f);
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlMars
	DF.ModelM->Create(DF::idSphere, "MdlMars", false, 48);
	DF.ModelM->SetMaterial("Mat_MarsPBS");
	DF.ModelM->SetPos(60.0f, -160.0f, 400.0f);
	DF.ModelM->SetScaling(260.0f, 260.0f, 260.0f);
	DF.ModelM->SetRotation(-0.4f, 1.4f, -0.4f);
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlMarsAtmo
	DF.ModelM->Create(DF::idSphere, "MdlMarsAtmo", true, 48);
	DF.ModelM->SetPos(60.0f, -160.0f, 400.0f);
	DF.ModelM->SetScaling(265.0f, 265.0f, 265.0f);
	DF.ModelM->SetMaterial("Mat_Atmo");
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlTachi
	DF.ModelM->Create(DF::idImport, "MdlTachi", true, "tachilp1.obj");
	DF.ModelM->SetShaders("VS_PBS_Shadow", "PS_PBS_Shadow");
	DF.ModelM->SetEffect(DF::Pass::Standard | DF::Pass::Shadow);
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetPos(0.0f, 0.0f, 8.0f);
	DF.ModelM->SetRotation(-0.6f, -0.4f, -0.3f);
	//
	
	//MdlCube1
	DF.ModelM->Create(DF::idCube, "MdlCube1");
	DF.ModelM->SetMaterial("Mat_PBSBricks");
	DF.ModelM->SetPos(1.0f, -1.0f, 2.0f);
	DF.ModelM->SetScaling(1.0f, 1.0f, 1.0f);
	DF.ModelM->SetRotation(0.8f, 1.2f, 0.0f);
	//*/
	
	//MdlSphere1
	DF.ModelM->Create(DF::idSphere, "MdlSphere1", true, 16);
	DF.ModelM->SetMaterial("Mat_PBSGold");
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(0.0f, 0.0f, 2.0f);
	
	//MdlSphere0
	DF.ModelM->Create(DF::idSphere, "MdlSphere0", true, 16);
	DF.ModelM->SetMaterial("Mat_PBSSteel");
	//DF.ModelM->SetEffect(DF::Layer::Outline, true, 0);
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(-0.7f, 0.0f, 2.0f);
	
	//MdlSphere2
	DF.ModelM->Create(DF::idSphere, "MdlSphere2", true, 16);
	DF.ModelM->SetMaterial("Mat_PBSMetal");
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(-1.4f, 0.0f, 2.0f);
	
	//MdlPoint
	DF.ModelM->Create(DF::idPoint, "MdlPoint");
	DF.ModelM->SetMaterial("Mat_SunP");
	DF.ModelM->SetEffect(DF::Pass::PointSprites);
	DF.ModelM->SetPos(-48.0f, 34.0f, -4.0f);
	//DF.ModelM->SetScaling(1.0f, 1.0f, 1.0f);
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlSun
	DF.ModelM->Create(DF::idSphere, "MdlSun", false, 12);
	DF.ModelM->Model().SetPos(-52.8f, 37.4f, -4.4f);
	DF.ModelM->SetMaterial("Mat_Sun");
	DF.ModelM->Model().FollowCamera(true);
	DF.ModelM->SetScaling(1.2f, 1.2f, 1.2f);

	//*/
	/*
	//MdlTestPlaneNoMip
	DF.ModelM->Create(DF::idPlane, "MdlPlaneTestNoMip");
	DF.ModelM->SetMaterial("Mat_TestNoMip");
	DF.ModelM->SetPos(-0.8f, 0.5f, 2.0f);
	DF.ModelM->SetScaling(0.1f, 0.1f, 0.1f);
	/*
	//MdlPlane1 (rtt test)
	DF.ModelM->Create(DF::idPlane, "MdlPlane1");
	DF.ModelM->SetMaterialRT("Mat_RTTDefault", 0u);
	DF.ModelM->SetPos(0.0f, 0.5f, 12.0f);
	DF.ModelM->SetScaling(3.2f, 1.8f, 0.0f);
	*/

	//DF.LMgr->ShowPLMeshes() = true;

	//DF.LightM->DLSetPos({ -24.0f, 17.0f, 0.0f });
	DF.LightM->DLSetPos({ -12.0f, 8.5f, -1.0f });
	//DF.LightM->DLSetPos({ 0.0f, 0.0f, -1.0f });
	
	DF.LightM->PLAdd("Light1", 0.17f, 0.515f, 8.55f);
	//DF.LightM->PL().pMesh->SetPos(0.0f, 0.0f, 0.0f);
	//DF.LightM->PL().pMesh->SetRotationZ(0.05f);
	DF.LightM->PL()->intensity = 0.07f;
	DF.LightM->PL()->color = { 1.0f, 0.02f, 0.02f, 1.0f };

	DF.LightM->PLAdd("Light2", -0.6f, 0.345f, 8.36f);
	//DF.LightM->PL().pMesh->SetRotationZ(-0.05f);
	DF.LightM->PL()->intensity = 0.07f;
	DF.LightM->PL()->color = { 1.0f, 0.02f, 0.02f, 1.0f };

	DF.LightM->PLAdd("Light3", 0.62f, -1.1f, 6.4f);
	DF.LightM->PL()->intensity = 0.2f;
	DF.LightM->PL()->color = { 0.0f, 0.25f, 1.0f, 1.0f };
	
	//
	
	DF.LightM->PLAdd("Light4", -0.7f, 0.25f, 2.0f);
	DF.LightM->PL()->intensity = 0.05f;
	DF.LightM->PL()->color = { 1.0f, 0.25f, 0.02f, 1.0f };
	
	DF.LightM->PLAdd("Light5", -0.35f, 0.5f, 2.0f);		//???? something wrong with distance algo
	DF.LightM->PL()->intensity = 0.08f;
	DF.LightM->PL()->color = { 0.02f, 1.02f, 0.02f, 1.0f };
	
	DF.LightM->PLAdd("Light6", -1.05f, -0.5f, 2.0f);
	DF.LightM->PL()->intensity = 0.05f;
	DF.LightM->PL()->color = { 1.02f, 0.02f, 0.02f, 1.0f };
	
	DF.ModelM->Select(0);
	DF.ModelM->Delete();
	DF.MatM->MatDelete("Mat_LoadScr");

	DF.Camera()->SetPos(-1.4f, 0.3f, 1.0f);			// test position
	//DF.Camera()->SetRotation(-7, 25);
	//DF.Camera()->SetPos(-1.03f, 1.47f, 9.69f);
	//DF.Camera()->SetRotation(-0.8f, 2.3f);

	// camera that will be used for shadows from directional light
	DF.CameraAdd("camLight");
	DF.CameraSelect("camLight");
	DF.Camera()->SetPos(DF.LightM->DLGetPosA());
	DF.LightM->DLSetCamera(DF.Camera("camLight"));

	DF.Camera()->SetAsOrthographic(8.0f, 4.5f, 0.001f, 100.0f);
	DF.Camera()->LookAtCameraTarget(DF.Camera("camMain"));
}
