//#include "RTechnique.h"
#include "../DFactory.h"

// RTECHNIQUEDB

void RTechniqueDB::InitDefaultTechniques() noexcept
{
	// do not initialize again
	if (m_InitializedDefaults)
	{
		return;
	}

	std::string VS, PS;
	uint8_t currentPass = 0;

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

	// PASS 0
	// renders to shadow depth from dir light's POV
	{
		fInitBinds();
		VS = "VS_Default";

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idSampler1] = std::make_unique<Bind::Sampler>(10u, 6u);
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Null_Texture>(6u);
		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_NONE);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = "";			// shadow render buffer (unused)
		m_Techniques.back().m_DSB = "dsCSM";	// shadow depth

		m_Techniques.back().m_Camera = "camLight";

		// write to stencil buffer with this pass
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::Default;

		// tells renderer to use it in a special cascade shadow pass, requires orthogonal camera and shadow depth buffer
		m_Techniques.back().m_IsCShadowTechnique = true;

		currentPass++;
	}

	// PASS 1
	// occlusion query testing
	{
		fInitBinds();

		VS = "VS_Default_OT";

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		//pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();

		std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(ied, pVSByteCode);
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Null_Texture>(6u);
		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = "";
		m_Techniques.back().m_DSB = "dsMain";

		m_Techniques.back().m_Camera = "$active_camera";

		// write to stencil buffer with this pass
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::Default;

		currentPass++;
	}

	// PASS 2
	// renders meshes to background
	{
		fInitBinds();

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = "rtMain";
		m_Techniques.back().m_DSB = "dsMain";

		m_Techniques.back().m_Camera = "$active_camera";

		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::DepthOff;

		currentPass++;
	}

	// PASS 3
	// use rendering using mesh own binds
	{
		fInitBinds();

		// create a bind to cascade shadow map
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->GetDepthTarget("dsCSM")->pDS_SRV.Get(), 6u);

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// set player camera for this and next passes until changed
		m_Techniques.back().m_Camera = "$active_camera";

		// generate light data for mesh
		m_Techniques.back().m_BindLights = true;

		// set render buffer and depth buffer for rendering to
		m_Techniques.back().m_RB = "rtMain";		// main buffer
		m_Techniques.back().m_DSB = "dsMain";		// main depth

		// depth stencil state writes to off
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::Default;

		currentPass++;
	}
	// PASS 4
	// use rendering using mesh own binds for 'blur' render buffer
	{
		fInitBinds();

		// create a bind to cascade shadow map
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->GetDepthTarget("dsCSM")->pDS_SRV.Get(), 6u);

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// generate light data for mesh
		m_Techniques.back().m_BindLights = true;

		// use same depth buffer from previous pass, but render to 'fxBlur' render buffer
		m_Techniques.back().m_RB = "rtBlur";	// fxBlur buffer
		m_Techniques.back().m_DSB = "dsMain";	// main depth

		// no change for depth state
		m_Techniques.back().m_depthState = -1;

		currentPass++;
	}
	// PASS 5 SPRITE LAYER
	//
	{
		fInitBinds();

		pBinds[Bind::idTopology] = std::make_unique<Bind::Topology>(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// set player camera for this and next passes until changed
		m_Techniques.back().m_Camera = "$active_camera";

		// set render buffer and depth buffer for rendering to
		m_Techniques.back().m_RB = "rtMain";	// main buffer
		m_Techniques.back().m_DSB = "dsMain";	// main depth

		currentPass++;
	}

	// PASS 6
	// debug show bounding meshes
	{
		fInitBinds();

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_Camera = "$active_camera";

		m_Techniques.back().m_RB = "rtMain";	// main buffer
		m_Techniques.back().m_DSB = "dsMain";	// main depth

		// no change for depth state
		m_Techniques.back().m_depthState = -1;

		currentPass++;
	}
	// PASS 7
	// write mask without actually drawing to render buffer
	{
		fInitBinds();
		VS = "VS_Default";

		pBinds[Bind::idRasterizer] = std::make_unique<Bind::Rasterizer>(D3D11_CULL_BACK, D3D11_FILL_SOLID);

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();

		//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);
		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = "rtMain";	// main buffer
		m_Techniques.back().m_DSB = "dsMain";	// main depth

		m_Techniques.back().m_Camera = "$active_camera";

		// write to stencil buffer with this pass
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::StencilWrite;

		currentPass++;
	}
	// PASS 8
	// masking pass
	{
		fInitBinds();
		VS = "VS_Outline_s2";
		PS = "PS_Outline_s2";

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + PS + ".shd");
		pBinds[Bind::idGeometryShader] = std::make_unique <Bind::Null_GeometryShader>();
		//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = "rtMain";	// main buffer
		m_Techniques.back().m_DSB = "dsMain";	// main depth

		// mask this pass with what's in stencil buffer currently
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Mode::StencilMask;

		currentPass++;
	}
	// mark default techniques as being initialized
	m_InitializedDefaults = true;
}

size_t RTechniqueDB::Size() const noexcept
{
	return m_Techniques.size();
}

RTechniqueDB::CascadeShadowMapData& RTechniqueDB::CSMData() noexcept
{
	return CSM_Data;
}

RTechniqueDB::CascadeShadowMapData::CascadeShadowMapData() noexcept
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
