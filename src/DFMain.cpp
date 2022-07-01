#include "pch.h"
#include "DFMain.h"

//PRIVATE

DFMain::DFMain(DFactory::DFACTORY_INIT_DESC* pDesc) : DF(DFactory::Init(pDesc))
{
}

void DFMain::LoadJSON(const wchar_t* path, json& out_j) noexcept
{
	std::ifstream fStream(path);

	if (!fStream.good()) {
		std::wstring msg = L"Failed to load '" + std::wstring(mapName.begin(), mapName.end()) + L".dmap'.\nThe map files are corrupt or missing.";
		MessageBoxW(nullptr, msg.c_str(), L"Critical Error", MB_OK | MB_ICONERROR);
		exit(404);
	}

	fStream >> out_j;
}

void DFMain::LoadMaterialsFromJSON(const nlohmann::json& materials) noexcept
{
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};

	for (const auto& it : materials)
	{
		// clear material description
		DFMatDesc = {};

		// get material name, must always be defined
		if (!it.contains("name")) {
			std::wstring msg = L"Failed to load '" + std::wstring(mapName.begin(), mapName.end()) + L"'\n.Definitions of materials are corrupt.";
			MessageBoxW(nullptr, L"Critical Error", msg.c_str(), MB_OK | MB_ICONERROR);
			exit(405);
		}
		DFMatDesc.name = it.at("name").get<std::string>();

		(it.contains("manageTextures")) ? it.at("manageTextures").get_to(DFMatDesc.manageTextures) : 0;

		// shaders
		if (it.contains("shaders")) {
			auto container = it.at("shaders");
			(container.contains("vs")) ? container.at("vs").get_to(DFMatDesc.shaders.vertex) : "";
			(container.contains("gs")) ? container.at("gs").get_to(DFMatDesc.shaders.geometry) : "";
			(container.contains("ps")) ? container.at("ps").get_to(DFMatDesc.shaders.pixel) : "";
		}

		// textures
		if (it.contains("textures")) {
			auto container = it.at("textures");
			(container.contains("tex0")) ? container.at("tex0").get_to(DFMatDesc.textures.tex0) : "";
			(container.contains("tex1")) ? container.at("tex1").get_to(DFMatDesc.textures.tex1) : "";
			(container.contains("tex2")) ? container.at("tex2").get_to(DFMatDesc.textures.tex2) : "";
			(container.contains("tex3")) ? container.at("tex3").get_to(DFMatDesc.textures.tex3) : "";
			(container.contains("tex4")) ? container.at("tex4").get_to(DFMatDesc.textures.tex4) : "";
			(container.contains("tex5")) ? container.at("tex5").get_to(DFMatDesc.textures.tex5) : "";
		}

		// variables
		if (it.contains("variables")) {
			auto container = it.at("variables");

			(container.contains("intensity")) ? container.at("intensity").get_to(DFMatDesc.material.intensity) : 0;
			(container.contains("bumpiness")) ? container.at("bumpiness").get_to(DFMatDesc.material.bumpiness) : 0;
			(container.contains("metalness")) ? container.at("metalness").get_to(DFMatDesc.material.metalness) : 0;
			(container.contains("roughness")) ? container.at("roughness").get_to(DFMatDesc.material.roughness) : 0;

			if (container.contains("ambient")) {
				float colors[3];
				container.at("ambient").get_to(colors);
				DFMatDesc.material.ambientColor = { colors[0], colors[1], colors[2], 1.0 };
			}

			if (container.contains("F0")) {
				float colors[3];
				container.at("F0").get_to(colors);
				DFMatDesc.material.F0 = { colors[0], colors[1], colors[2] };
			}
		}

		// passes
		if (it.contains("passes")) {
			auto passes = it.at("passes");
			for (const auto& pass : passes) {

				std::string passName = pass.get<std::string>();

				(passName == "background") ? DFMatDesc.passes |= DF::Pass::Background : 0;
				(passName == "standard") ? DFMatDesc.passes |= DF::Pass::Standard : 0;
				(passName == "blur") ? DFMatDesc.passes |= DF::Pass::Blur : 0;
				(passName == "sprite") ? DFMatDesc.passes |= DF::Pass::PointSprites : 0;
				(passName == "shadow") ? DFMatDesc.passes |= DF::Pass::Shadow : 0;
				(passName == "null") ? DFMatDesc.passes = 0 : 0;
			}
		}

		DF.MatM->MatAdd(&DFMatDesc);
	}
}

void DFMain::DrawFrame()
{
	//deltaA += 0.002f;

	DF.ModelM->Select("MdlTachi");
	//DF.ModelM->SetRotation(-0.6f - deltaA*0.7f , -0.4f + deltaA, -0.3f + deltaA * 0.7f);

	//DF.LightM->PL(0).pMesh->DEBUG_Rotate(0.01f);
	//DF.LightM->PL(1).pMesh->DEBUG_Rotate(-0.01f);

	DF.BeginFrame();

	DF.DrawFrame();
	
	if (DF.DevUIMode())
	{
		DF.ShowEditModelWindow();
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
	LoadMap(DF::defaultMap);

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
	json jsonData;
	const std::wstring mapPath = L"maps/" + map + L".dmap/";
	const std::wstring initPath = mapPath + L"init.json";
	const std::wstring matPath = mapPath + L"materials.json";
	const std::wstring objPath = mapPath + L"objects.json";

	// loading "load screen" data
	LoadJSON(initPath.c_str(), jsonData);

	// parse loaded JSON file and get materials data
	LoadMaterialsFromJSON(jsonData["materials"]);

	// get load screen material name
	std::string loadScreenMat = jsonData.at("loadScreen").at("useMaterial").get<std::string>();

	// start rendering loading screen
	DF::isLoadScreen = true;

	DF.MatM->BindTextureToPS(DF.MatM->Mat(DF.MatM->MatIndex(loadScreenMat)).idTex[0], 0u);

	DF.BeginFrame();

	DF.pD3DMgr->RTBind("rtBack", "dsBack");

	DF.pD3DMgr->RenderSurface("sfcMain");

	DF.EndFrame();
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
	DFMatDesc.textures.tex0 = "PBR/iron/rustediron2_basecolor.dds";
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
	DF.LightM->PL().intensity = 0.07f;
	DF.LightM->PL().color = { 1.0f, 0.02f, 0.02f, 1.0f };

	DF.LightM->PLAdd("Light2", -0.6f, 0.345f, 8.36f);
	//DF.LightM->PL().pMesh->SetRotationZ(-0.05f);
	DF.LightM->PL().intensity = 0.07f;
	DF.LightM->PL().color = { 1.0f, 0.02f, 0.02f, 1.0f };

	DF.LightM->PLAdd("Light3", 0.62f, -1.1f, 6.4f);
	DF.LightM->PL().intensity = 0.2f;
	DF.LightM->PL().color = { 0.0f, 0.25f, 1.0f, 1.0f };
	
	//
	
	DF.LightM->PLAdd("Light4", -0.7f, 0.25f, 2.0f);
	DF.LightM->PL().intensity = 0.05f;
	DF.LightM->PL().color = { 1.0f, 0.25f, 0.02f, 1.0f };
	
	DF.LightM->PLAdd("Light5", -0.35f, 0.5f, 2.0f);		//???? something wrong with distance algo
	DF.LightM->PL().intensity = 0.08f;
	DF.LightM->PL().color = { 0.02f, 1.02f, 0.02f, 1.0f };
	
	DF.LightM->PLAdd("Light6", -1.05f, -0.5f, 2.0f);
	DF.LightM->PL().intensity = 0.05f;
	DF.LightM->PL().color = { 1.02f, 0.02f, 0.02f, 1.0f };
	
	DF.ModelM->Select(0);
	DF.ModelM->Delete();
	DF.MatM->MatDelete(1);

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
	DF.Camera()->LockToCameraTarget(DF.Camera("camMain"));
}
