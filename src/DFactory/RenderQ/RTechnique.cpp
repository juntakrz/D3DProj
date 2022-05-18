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
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();
		pBinds[Bind::idSampler1] = std::make_unique<Bind::Sampler>(3u, 1u);
		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Null_Texture>(6u);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = 4u;			// render buffer
		m_Techniques.back().m_DSB = 3u;			// shadow depth

		m_Techniques.back().m_Camera = "camLight";

		// write to stencil buffer with this pass
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Stencil::Off;

		currentPass++;
	}
	// PASS 1
	// use rendering using mesh own binds
	{
		fInitBinds();

		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->DepthTargets()->at(3u).pDS_SRV.Get(), 6u);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// set player camera for this and next passes until changed
		m_Techniques.back().m_Camera = "$active_camera";

		// set render buffer and depth buffer for rendering to
		m_Techniques.back().m_RB = 1u;			// render buffer
		m_Techniques.back().m_DSB = 1u;			// render depth

		// depth stencil state writes to off
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Stencil::Off;

		currentPass++;
	}
	// PASS 2
	// use rendering using mesh own binds for 'blur' render buffer
	{
		fInitBinds();

		pBinds[Bind::idTextureDepth] = std::make_unique<Bind::Texture>(DF::D3DM->DepthTargets()->at(3u).pDS_SRV.Get(), 6u);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_MESH_AND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// use same depth buffer from previous pass, but render to 'fxBlur' render buffer
		m_Techniques.back().m_RB = 2u;			// fxBlur buffer
		m_Techniques.back().m_DSB = 1u;			// render depth

		// no change for depth state
		m_Techniques.back().m_depthState = -1;

		currentPass++;
	}
	// PASS 3
	// write mask without actually drawing to render buffer
	{
		fInitBinds();
		VS = "VS_Default";

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();

		//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);
		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		m_Techniques.back().m_RB = 1u;			// render buffer
		m_Techniques.back().m_DSB = 1u;			// render depth

		m_Techniques.back().m_Camera = "$active_camera";

		// write to stencil buffer with this pass
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Stencil::Write;

		currentPass++;
	}
	// PASS 4
	// masking pass
	{
		fInitBinds();
		VS = "VS_Outline_s2";
		PS = "PS_Outline_s2";

		auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
		ID3DBlob* pVSByteCode = pVS->GetByteCode();
		pBinds[Bind::idVertexShader] = std::move(pVS);
		pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + PS + ".shd");
		//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

		pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);

		m_Techniques.emplace_back(RTechnique());
		m_Techniques.back().m_Id = 1 << currentPass;
		m_Techniques.back().m_BindMode = RTechnique::BIND_TECHNIQUE;
		m_Techniques.back().m_Binds = std::move(pBinds);

		// mask this pass with what's in stencil buffer currently
		m_Techniques.back().m_depthState = (uint8_t)DF::DS_Stencil::Mask;

		currentPass++;
	}
	// mark default techniques as being initialized
	m_InitializedDefaults = true;
}

size_t RTechniqueDB::Size() const noexcept
{
	return m_Techniques.size();
}
