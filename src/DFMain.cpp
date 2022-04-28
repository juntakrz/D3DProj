#include "pch.h"
#include "DFMain.h"

GDIPlusMgr gdipm;

//PRIVATE

DFMain::DFMain(DFactory::DFACTORY_INIT_DESC* pDesc) : DF(DFactory::Init(pDesc))
{
}

void DFMain::DrawFrame()
{
	deltaA += 0.0001f;
	
	// MdlStars
	DF.ModelM->Select(2);
	DF.ModelM->SetPos(DF.Camera()->GetPos().x, DF.Camera()->GetPos().y, DF.Camera()->GetPos().z);
	
	// MdlMars
	DF.ModelM->Select(1);
	DF.ModelM->SetPos(DF.Camera()->GetPos().x + 600.0f, DF.Camera()->GetPos().y - 1600.0f, DF.Camera()->GetPos().z + 4000.0f);
	//DF.ModelM->SetRotation(-0.4f, deltaA, -0.4f);

	DF.LightM->PL(0).pMesh->DEBUG_Rotate(0.01f);
	DF.LightM->PL(1).pMesh->DEBUG_Rotate(-0.01f);

	DF.BeginFrame();

	DF.DrawFrame();
	
	if (ImGui::Begin("DIRECT3D11 DEMO"))
	{
		float simspeed = DF.GetSimulationSpeed();
		ImGui::Text("\nStatistics\n");
		ImGui::Text("%.1f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::SliderFloat("Simulation Speed", &simspeed, 0.25f, 4.0f, "%.1f");

		DF.SetSimulationSpeed(simspeed);
	}
	ImGui::End();

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
	DFMatDesc.shaders.vertex = "VS_BasicTexture";
	DFMatDesc.shaders.pixel = "PS_BasicTexture";
	DFMatDesc.textures.base = "img_loadscr.png";
	DF.MatM->MatAdd(&DFMatDesc);

	auto Animate = [&]() {
		//DF.Mesh(0)->TranslateZ(0.12);
		//DF.BeginFrame();
		//DF.DrawFrame();
		//DF.EndFrame();
	};

	DF.BeginFrame();
	
	DF.ModelM->Create(DF::idPlane, "MdlLoadScr");
	DF.ModelM->SetMaterial("Mat_LoadScr");
	DF.ModelM->SetScaling(16.0f, 9.0f, 0.0f);
	DF.ModelM->SetPos(0.0f, 0.0f, 16.0f);
	
	DF.DrawFrame();

	DF.EndFrame();
	
	//Mat_Mars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Mars";
	DFMatDesc.shaders.vertex = "VS_Standard";
	DFMatDesc.shaders.pixel = "PS_Standard";
	DFMatDesc.textures.base = "mars_4k_color.png";
	DFMatDesc.textures.normal = "mars_4k_normal.png";
	DFMatDesc.material.ambientColor = { 0.06f, 0.02f, 0.0f, 0.0f };
	DFMatDesc.material.matIntensity = 2.9f;
	DFMatDesc.material.spec_metal = 0.0f;
	DFMatDesc.material.pow_roughness = 0.0f;

	DF.MatM->MatAdd(&DFMatDesc);

	Animate();
	
	//Mat_Stars
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Stars";
	DFMatDesc.shaders.vertex = "VS_BasicTexture";
	DFMatDesc.shaders.pixel = "PS_BasicTexture";
	DFMatDesc.textures.base = "stars_6k_color.png";
	DFMatDesc.material.matIntensity = 0.1f;

	DF.MatM->MatAdd(&DFMatDesc);

	Animate();

	//Mat_PBSMetal
	DFMatDesc = {};
	DFMatDesc.name = "Mat_PBSMetal";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS";
	DFMatDesc.textures.base = "PBR//iron//rustediron2_basecolor.png";
	DFMatDesc.textures.normal = "PBR//iron//rustediron2_normal.png";
	DFMatDesc.textures.tex2 = "PBR//iron//rustediron2_metallic.png";
	DFMatDesc.textures.tex3 = "PBR//iron//rustediron2_roughness.png";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.12f, 1.0f };
	DFMatDesc.material.spec_metal = 1.0f;
	DFMatDesc.material.pow_roughness = 1.0f;

	DF.MatM->MatAdd(&DFMatDesc);

	//Mat_Metal
	DFMatDesc = {};
	DFMatDesc.name = "Mat_Metal";
	DFMatDesc.shaders.vertex = "VS_PBS";
	DFMatDesc.shaders.pixel = "PS_PBS_NoHDR";
	//DFMatDesc.shaders.vertex = "VS_Standard";
	//DFMatDesc.shaders.pixel = "PS_Standard";
	DFMatDesc.textures.base = "metal1.png";
	DFMatDesc.textures.normal = "metal1_normal.png";
	DFMatDesc.material.ambientColor = { 0.0f, 0.0f, 0.12f, 1.0f };
	DFMatDesc.material.spec_metal = 0.2f;		//1.5
	DFMatDesc.material.pow_roughness = 0.1f;	//1.2
	DFMatDesc.material.F0 = { 0.08, 0.07, 0.06 };
	DFMatDesc.material.extra = 0.0f;

	DF.MatM->MatAdd(&DFMatDesc);

	Animate();
	
	//MdlMars
	DF.ModelM->Create(DF::idSphere, "MdlMars", 64);
	DF.ModelM->SetMaterial("Mat_Mars");
	DF.ModelM->SetPos(600.0f, -1600.0f, 4000.0f);
	DF.ModelM->SetScaling(2800.0f, 2800.0f, 2800.0f);
	DF.ModelM->SetRotation(-0.4f, 0.0f, -0.4f);
	//DF.Mesh()->SetRotationY(-0.00005f);
	
	Animate();

	//MdlStars
	DF.ModelM->Create(DF::idSkySphere, "MdlStars");
	DF.ModelM->SetMaterial("Mat_Stars");
	DF.ModelM->SetScaling(3700.0f, 3700.0f, 3700.0f);
	DF.ModelM->SetRotation(0.0f, 2.2f, -0.4f);
	//DF.Mesh()->SetRotationY(0.00002f);
	
	Animate();
	/*
	DF.ModelM->Create(DF::idImport, "MdlTachi", "tachilp1.obj");
	DF.ModelM->SetShaders("VS_Classic", "PS_Classic");
	DF.ModelM->SetScaling(0.5f, 0.5f, 0.5f);
	DF.ModelM->SetPos(0.0f, -30.0f, 70.0f);
	DF.ModelM->SetRotation(-0.6f, -0.4f, -0.3f);
	*/
	/*
	DF.ModelM->Create(DF::idImport, "MdlRoci", "Roci2.obj");
	DF.ModelM->SetShaders("VS_BumpML", "PS_BumpML");
	DF.ModelM->SetPos(30.0f, -50.0f, 40.0f);
	DF.ModelM->SetScaling(44.0f, 44.0f, 44.0f);
	DF.ModelM->SetRotation(-DirectX::XM_PI - 0.6f, -0.4f, -DirectX::XM_PI + 0.3f);
	*/
	
	DF.ModelM->Create(DF::idSphere, "MdlSphere1", 64);
	DF.ModelM->SetScaling(0.8f, 0.8f, 0.8f);
	DF.ModelM->SetRotation(0.5f, 0.5f, 0.5f);
	DF.ModelM->SetPos(0.0f, 0.0f, 8.0f);
	DF.ModelM->SetMaterial("Mat_PBSMetal");
	
	Animate();
	
	//DF.LMgr->ShowPLMeshes() = true;
	
	DF.LightM->PLAdd("Light1", 0.8f, -0.2f, 8.0f);
	//DF.LightM->PL().pMesh->SetRotationZ(0.05f);
	DF.LightM->PL().intensity = 0.1f;
	DF.LightM->PL().color = { 1.0f, 0.4f, 0.0f, 1.0f };

	Animate();

	DF.LightM->PLAdd("Light2", -1.0f, 0.4f, 8.0f);
	//DF.LightM->PL().pMesh->SetRotationZ(-0.05f);
	DF.LightM->PL().intensity = 0.3f;
	DF.LightM->PL().color = { 1.0f, 0.4f, 0.0f, 1.0f };

	Animate();

	DF.LightM->PLAdd("Light3", 0.0f, -2.0f, 6.0f);
	DF.LightM->PL().intensity = 1.0f;
	DF.LightM->PL().color = { 0.0f, 0.2f, 1.0f, 1.0f };

	Animate();

	DF.ModelM->Select(0);
	DF.ModelM->Delete();

	DF.Camera()->SetPos(0.0f, 0.0f, 4.0f);
}

void DFMain::CreateMaterials() noexcept
{
	//
}
