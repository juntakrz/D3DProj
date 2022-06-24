//#include "RTechnique.h"
#include "../DFactory.h"

// RTECHNIQUEDB

void RTechnique::InitTechniques() noexcept
{
	// do not initialize again
	if (m_InitializedDefaults)
	{
		return;
	}

	std::string VS, PS;

	// initialize bind vector and its standard 24 slots
	std::vector<std::unique_ptr<Bind::IBind>> pBinds;

	// init binds after moving
	auto fInitBinds = [&]()
	{
		for (uint8_t i = 0; i < Bind::MAXBINDS; i++)
		{
			pBinds.emplace_back(nullptr);
		}
	};

	// CONCEPT: 'Pass X' 'Intended pass use description'

	// PASS: Shadow
	// renders to shadow depth from dir light's POV
	{
		fInitBinds();
		VS = "VS_Default";

		auto pVS = std::make_unique<Bind::VertexShader>(VS);
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idSampler1] = std::make_unique<Bind::Sampler>(10u, 6u);
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Null_Texture>(6u);
		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_NONE);

		std::string techId = DF::Pass::IdToString(DF::Pass::Shadow);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_RB = "";			// shadow render buffer (unused)
		m_TechDB[techId].m_DSB = "dsCSM";	// shadow depth

		m_TechDB[techId].m_Camera = "camLight";

		// write to stencil buffer with this pass
		m_TechDB[techId].m_depthState = DF::DS_Mode::Default;

		// mark technique as suitable for cascade shadow mapping pass
		m_TechDB[techId].m_IsCSM = true;
	}

	// PASS: Occlusion
	// occlusion query testing
	{
		fInitBinds();

		VS = "VS_Default_OT";
		PS = "PS_Default";
		//VS = "VS_Default";

		auto pVS = std::make_unique<Bind::VertexShader>(VS);
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		//pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(PS);
		//pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Null_Texture>(6u);
		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		auto techId = DF::Pass::IdToString(DF::Pass::Occlusion);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_RB = ""; // ""
		m_TechDB[techId].m_DSB = "dsMain";

		m_TechDB[techId].m_Camera = "$active_camera";

		// write to stencil buffer with this pass
		m_TechDB[techId].m_depthState = (uint8_t)DF::DS_Mode::Default;
	}

	// PASS: Background
	// renders meshes to background
	{
		fInitBinds();

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		auto techId = DF::Pass::IdToString(DF::Pass::Background);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_RB = "rtMain";
		m_TechDB[techId].m_DSB = "dsMain";

		m_TechDB[techId].m_Camera = "$active_camera";

		m_TechDB[techId].m_BindLights = true;

		m_TechDB[techId].m_depthState = DF::DS_Mode::DOff_SOff;
	}

	// PASS: Standard
	// use rendering using mesh own binds
	{
		fInitBinds();

		// create a bind to cascade shadow map
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->GetDepthTarget("dsCSM")->pDepthSRV.Get(), 6u);

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		auto techId = DF::Pass::IdToString(DF::Pass::Standard);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		// set player camera for this and next passes until changed
		m_TechDB[techId].m_Camera = "$active_camera";

		// generate light data for mesh
		m_TechDB[techId].m_BindLights = true;

		// set render buffer and depth buffer for rendering to
		m_TechDB[techId].m_RB = "rtMain";		// main buffer
		m_TechDB[techId].m_DSB = "dsMain";		// main depth

		// depth stencil state writes to off
		m_TechDB[techId].m_depthState = DF::DS_Mode::Default;
	}
	// PASS: Blur
	// use rendering using mesh own binds for 'blur' render buffer
	{
		fInitBinds();

		// create a bind to cascade shadow map
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->GetDepthTarget("dsCSM")->pDepthSRV.Get(), 6u);

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		auto techId = DF::Pass::IdToString(DF::Pass::Blur);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		// generate light data for mesh
		m_TechDB[techId].m_BindLights = true;

		// use same depth buffer from previous pass, but render to 'fxBlur' render buffer
		m_TechDB[techId].m_RB = "rtBlur";	// fxBlur buffer
		m_TechDB[techId].m_DSB = "dsBlur";	// main depth

		// no change for depth state
		m_TechDB[techId].m_depthState = -1;
		//m_TechDB[techId].m_depthState = DF::DS_Mode::DOn_SWrite;
	}
	// PASS 5 SPRITE LAYER
	//
	{
		fInitBinds();

		pBinds[Bind::idSampler1] = std::make_unique<Bind::Sampler>(Bind::Sampler::Comparison, 1u, DF::ShaderType::GS);
		pBinds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		auto techId = DF::Pass::IdToString(DF::Pass::PointSprites);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_Binds = std::move(pBinds);

		// set player camera for this and next passes until changed
		m_TechDB[techId].m_Camera = "$active_camera";

		// set render buffer and depth buffer for rendering to
		m_TechDB[techId].m_RB = "rtMix";	// main buffer
		m_TechDB[techId].m_DSB = "dsMain";	// main depth

		// depth stencil state writes to off
		m_TechDB[techId].m_depthState = DF::DS_Mode::Default;
	}

	// PASS 6
	// debug show bounding meshes
	{
		fInitBinds();

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);

		auto techId = DF::Pass::IdToString(DF::Pass::AABBShow);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_Camera = "$active_camera";

		m_TechDB[techId].m_RB = "rtMain";	// main buffer
		m_TechDB[techId].m_DSB = "dsMain";	// main depth

		// no change for depth state
		m_TechDB[techId].m_depthState = -1;
	}
	// PASS 7
	// write mask without actually drawing to render buffer
	{
		fInitBinds();
		VS = "VS_Default";

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK, D3D11_FILL_SOLID);

		auto pVS = std::make_unique<Bind::VertexShader>(VS);
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();

		auto techId = DF::Pass::IdToString(DF::Pass::Masking);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_RB = "rtMain";	// main buffer
		m_TechDB[techId].m_DSB = "dsMain";	// main depth

		m_TechDB[techId].m_Camera = "$active_camera";

		// write to stencil buffer with this pass
		m_TechDB[techId].m_depthState = DF::DS_Mode::DOff_SWrite;
	}
	// PASS 8
	// masking pass
	{
		fInitBinds();
		VS = "VS_Outline_s2";
		PS = "PS_Outline_s2";

		auto pVS = std::make_unique<Bind::VertexShader>(VS);
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(PS);
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();
		//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);

		auto techId = DF::Pass::IdToString(DF::Pass::Outline);
		m_TechDB[techId] = TechniqueData();

		m_TechDB[techId].m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_TechDB[techId].m_Binds = std::move(pBinds);

		m_TechDB[techId].m_RB = "rtMain";	// main buffer
		m_TechDB[techId].m_DSB = "dsMain";	// main depth

		// mask this pass with what's in stencil buffer currently
		m_TechDB[techId].m_depthState = DF::DS_Mode::DOff_SRead;
	}
	// mark default techniques as being initialized
	m_InitializedDefaults = true;
}

RTechnique::CascadeShadowMapData& RTechnique::CSMData() noexcept
{
	return CSM_Data;
}

RTechnique::CascadeShadowMapData::CascadeShadowMapData() noexcept
{
	{
		float coreZ = DF::CSM::maxZ / DF::CSM::cascades;

		for (uint8_t index = 0; index < DF::CSM::cascades; index++)
		{
			// precalculate depths
			//cascadeData.nearZ.emplace_back((index == 0) ? DF::minZ : coreZ * index);
			cascadeData.nearZ.emplace_back(DF::CSM::minZ);
			cascadeData.farZ.emplace_back(DF::CSM::maxZ);

			// create viewport entry
			vp.emplace_back();

			// fill viewport data
			vp[index].TopLeftX = DF::CSM::bufferSize * index;
			vp[index].TopLeftY = 0.0f;
			vp[index].Width = DF::CSM::bufferSize;
			vp[index].Height = DF::CSM::bufferSize;
			vp[index].MinDepth = 0.0f;
			vp[index].MaxDepth = 1.0f;
		}
	}
}
