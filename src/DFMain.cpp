#include "pch.h"
#include "DFMain.h"

//PRIVATE

DFMain::DFMain(DFactory::DFACTORY_INIT_DESC* pDesc) : DF(DFactory::Init(pDesc))
{
}

void DFMain::DrawFrame()
{
	deltaA += 0.002f;

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
	//DF.font = 

	LoadScreen();

	// -------
	/*
	std::mt19937 randomSeed(std::random_device{}());
	std::uniform_real_distribution<float> aDist(0.0f, 2 * PI32);
	std::uniform_real_distribution<float> dDist(0.0f, 2 * PI32);
	std::uniform_real_distribution<float> oDist(0.0f, 0.3 * PI32);
	std::uniform_real_distribution<float> rDist(6.0f, 20.0f);
	*/

	/*CSurface texture(1024, 1024);
	std::uniform_int_distribution starDist(0xFF000000u, 0xFFFFFFFFu);
	std::uniform_int_distribution colorDist(0xFF22AACCu, 0xFFFFFFFFu);
	for (int x = 0; x < 1024; x++)
	{
		for (int y = 0; y < 1024; y++)
		{
			int dword = starDist(randomSeed);
			(dword < 0xFFFEEFFFu) ? dword = 0xFF000000u : dword = colorDist(randomSeed);
			texture.SetPixel(x, y, CSurface::Color(dword));
		}
	}

	texture.SaveToFile(L"test.png");*/

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

void DFMain::LoadScreen() noexcept
{
	DFMaterial::DFMATERIAL_DESC DFMatDesc{};

	//Mat_LoadScr
	DFMatDesc.name = "Mat_LoadScr";
	DFMatDesc.shaders.vertex = "VS_FlatTexture";
	DFMatDesc.shaders.pixel = "PS_FlatTexture";
	DFMatDesc.textures.tex0 = "img_loadscr.dds";
	DFMatDesc.manageTextures = true;

	DF.MatM->MatAdd(&DFMatDesc);

	DF.BeginFrame();
	
	DF.ModelM->Create(DF::idPlane, "MdlLoadScr", false);
	DF.ModelM->SetMaterial("Mat_LoadScr");
	DF.ModelM->SetScaling(16.0f, 9.0f, 0.0f);
	DF.ModelM->SetPos(0.0f, 0.0f, 16.5f);

	//DF.UpdateRenderer();
	
	DF.DrawFrame();

	DF.EndFrame();
	
	//Mat_Mars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Mars";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS_NoHDR";
	DFMatDesc.textures.tex0 = "PBR//mars//mars_4k_albedo.dds";
	DFMatDesc.textures.tex1 = "PBR//mars//mars_4k_normal.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.06f, 0.0f, 0.0f };
	DFMatDesc.material.matIntensity = 2.9f;	//2.9
	DFMatDesc.material.spec_metal = 0.0f;
	DFMatDesc.material.pow_roughness = 0.0f;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_MarsPBS
	DFMatDesc = {};
	DFMatDesc.name = "Mat_MarsPBS";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS_NoHDR_L";
	DFMatDesc.textures.tex0 = "PBR//mars//mars_4k_albedo.dds";
	DFMatDesc.textures.tex1 = "PBR//mars//mars_4k_normal.dds";
	DFMatDesc.textures.tex2 = "PBR//mars//mars_4k_metallic.dds";
	DFMatDesc.textures.tex3 = "PBR//mars//mars_4k_roughness.dds";
	DFMatDesc.textures.tex4 = "PBR//mars//mars_4k_ao.dds";
	DFMatDesc.material.ambientColor = { 0.06f, 0.03f, 0.0f, 1.0f };
	DFMatDesc.material.spec_metal = 0.08f;
	DFMatDesc.material.pow_roughness = 0.8f;
	DFMatDesc.material.F0 = { 0.25f, 0.23f, 0.00f };
	DFMatDesc.material.matIntensity = 2.2f;
	DFMatDesc.material.bumpIntensity = 1.5f;
	DFMatDesc.effects = DF::fxBackground;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_Atmo
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Atmo";
	DFMatDesc.shaders.vertex = "VS_Atmosphere";
	DFMatDesc.shaders.pixel = "PS_Atmosphere";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.15f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;
	DFMatDesc.material.matIntensity = 0.9f;
	DFMatDesc.effects = DF::fxBlur;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//Mat_Stars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Stars";
	DFMatDesc.shaders.vertex = "VS_FlatTexture";
	DFMatDesc.shaders.pixel = "PS_FlatTexture";
	DFMatDesc.textures.tex0 = "stars_6k_color.dds";
	DFMatDesc.material.matIntensity = 0.1f;
	DFMatDesc.effects = DF::fxBackground;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSMetal
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSMetal";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR//iron//rustediron2_basecolor.dds";
	DFMatDesc.textures.tex1 = "PBR//iron//rustediron2_normal.dds";
	DFMatDesc.textures.tex2 = "PBR//iron//rustediron2_metallic.dds";
	DFMatDesc.textures.tex3 = "PBR//iron//rustediron2_roughness.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.0f, 0.0f, 0.0f };
	DFMatDesc.material.bumpIntensity = 2.0f;
	DFMatDesc.effects = DF::fxStandard | DF::fxShadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSGold
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSGold";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR//gold_ornate//albedo.dds";
	DFMatDesc.textures.tex1 = "PBR//gold_ornate//normal.dds";
	DFMatDesc.textures.tex2 = "PBR//gold_ornate//metallic.dds";
	DFMatDesc.textures.tex3 = "PBR//gold_ornate//roughness.dds";
	DFMatDesc.textures.tex4 = "PBR//gold_ornate//ao.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpIntensity = 2.0f;
	DFMatDesc.effects = DF::fxStandard | DF::fxShadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_PBSSteel
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSSteel";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR//steel_plate//albedo.dds";
	DFMatDesc.textures.tex1 = "PBR//steel_plate//normal.dds";
	DFMatDesc.textures.tex2 = "PBR//steel_plate//metallic.dds";
	DFMatDesc.textures.tex3 = "PBR//steel_plate//roughness.dds";
	DFMatDesc.textures.tex4 = "PBR//steel_plate//ao.dds";
	DFMatDesc.material.ambientColor = { 0.1f, 0.1f, 0.01f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpIntensity = 2.0f;
	DFMatDesc.effects = DF::fxBlur | DF::fxShadow;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_Test
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Test";
	DFMatDesc.shaders.vertex = "VS_PBS_Shadow";
	DFMatDesc.shaders.pixel = "PS_PBS_Shadow";
	DFMatDesc.textures.tex0 = "PBR//gold_ornate//albedo.dds";
	DFMatDesc.textures.tex1 = "PBR//gold_ornate//normal.dds";
	DFMatDesc.textures.tex2 = "PBR//gold_ornate//metallic.dds";
	DFMatDesc.textures.tex3 = "PBR//gold_ornate//roughness.dds";
	DFMatDesc.textures.tex4 = "PBR//gold_ornate//ao.dds";
	DFMatDesc.material.ambientColor = { 0.2f, 0.2f, 0.02f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;
	DFMatDesc.material.F0 = { 0.25f, 0.25f, 0.05f };
	DFMatDesc.material.bumpIntensity = 2.0f;
	DFMatDesc.effects = DF::fxStandard | DF::fxShadow;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//Mat_Sun
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Sun";
	DFMatDesc.shaders.vertex = "VS_BB_Flat";
	DFMatDesc.shaders.pixel = "PS_BB_FlatBlend";
	DFMatDesc.textures.tex0 = "default//sun01.dds";
	DFMatDesc.effects = DF::fxBackground;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_OTest
	DFMatDesc = {};
	DFMatDesc.name = "Mat_OTest";
	DFMatDesc.shaders.vertex = "VS_Default";
	DFMatDesc.shaders.pixel = "PS_Default";
	DFMatDesc.effects = DF::fxBackground;

	DF.MatM->MatAdd(&DFMatDesc);
	
	//MdlStars
	DF.ModelM->Create(DF::idSkySphere, "MdlStars");
	DF.ModelM->SetMaterial("Mat_Stars");
	DF.ModelM->SetScaling(400.0f, 400.0f, 400.0f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlMars
	DF.ModelM->Create(DF::idSphere, "MdlMars", false, 64);
	DF.ModelM->SetMaterial("Mat_MarsPBS");
	DF.ModelM->SetPos(60.0f, -160.0f, 400.0f);
	DF.ModelM->SetScaling(260.0f, 260.0f, 260.0f);
	DF.ModelM->SetRotation(-0.4f, 1.4f, -0.4f);
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlMarsAtmo
	DF.ModelM->Create(DF::idSphere, "MdlMarsAtmo", true, 64);
	DF.ModelM->SetPos(60.0f, -160.0f, 400.0f);
	//DF.ModelM->SetEffect(DF::fxOutline, true, 0); // not implemented yet
	DF.ModelM->SetScaling(265.0f, 265.0f, 265.0f);
	DF.ModelM->SetMaterial("Mat_Atmo");
	DF.ModelM->Model().FollowCamera(true);
	
	//MdlTachi
	DF.ModelM->Create(DF::idImport, "MdlTachi", true, "tachilp1.obj");
	DF.ModelM->SetShaders("VS_PBS_Shadow", "PS_PBS_Shadow");
	DF.ModelM->SetEffect(DF::fxStandard | DF::fxShadow);
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetPos(0.0f, 0.0f, 8.0f);
	DF.ModelM->SetRotation(-0.6f, -0.4f, -0.3f);
	//
	
	//MdlCube1
	DF.ModelM->Create(DF::idCube, "MdlCube1");
	DF.ModelM->SetMaterial("Mat_Test");
	DF.ModelM->SetPos(1.0f, -1.0f, 2.0f);
	DF.ModelM->SetScaling(1.0f, 1.0f, 1.0f);
	DF.ModelM->SetRotation(0.8f, 1.2f, 0.0f);
	//*/
	//MdlSphere1
	DF.ModelM->Create(DF::idSphere, "MdlSphere1");
	DF.ModelM->SetMaterial("Mat_PBSGold");
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(0.0f, 0.0f, 2.0f);
	
	//MdlSphere0
	DF.ModelM->Create(DF::idSphere, "MdlSphere0");
	DF.ModelM->SetMaterial("Mat_PBSSteel");
	//DF.ModelM->SetEffect(DF::fxOutline, true, 0);
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(-0.7f, 0.0f, 2.0f);
	
	//MdlSphere2
	DF.ModelM->Create(DF::idSphere, "MdlSphere2");
	DF.ModelM->SetMaterial("Mat_PBSMetal");
	DF.ModelM->SetScaling(0.2f, 0.2f, 0.2f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	DF.ModelM->SetPos(-1.4f, 0.0f, 2.0f);
	
	//MdlPoint
	DF.ModelM->Create(DF::idPoint, "MdlPoint");
	DF.ModelM->SetMaterial();
	DF.ModelM->SetPos(0.0f, 0.0f, 3.0f);
	
	//MdlSun
	DF.ModelM->Create(DF::idPlane, "MdlSun", false);
	DF.ModelM->Model().SetPos(DF.LightM->DLGetPosA());
	DF.ModelM->SetMaterial("Mat_Sun");
	DF.ModelM->Model().FollowCamera(true);
	DF.ModelM->SetScaling(3.0f, 3.0f, 1.0f);

	//MdlOTest
	DF.ModelM->Create(DF::idSphere, "MdlOTest", false);
	DF.ModelM->Model().SetPos(DF.LightM->DLGetPosA());
	DF.ModelM->SetScaling(0.04f, 0.04f, 0.04f);
	DF.ModelM->Model().FollowCamera(true);
	DF.ModelM->SetMaterial("Mat_OTest");
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
	/*
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
	*/
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

	//DF.UpdateRenderer();
}

void DFMain::CreateMaterials() noexcept
{
	//
}
