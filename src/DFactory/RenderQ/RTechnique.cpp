#include "RTechnique.h"

// RPASSSTEP

void RTechniqueStep::StepBind() noexcept
{
	for (const auto& it : m_FXBinds)
	{
		it ? it->Bind() : void();
	}
}

// RTECHNIQUE

RTechnique::RTechnique(uint32_t id) noexcept : m_id(id)
{
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

	switch (m_id)
	{
	case(DF::fxOutline) :
	{
		//
		// STEP FOR PASS 1
		//

		// should get these from technique related material in the future
		{
			fInitBinds();
			VS = "VS_Default";
			//PS = "PS_Default";
			
			auto pVS = std::make_unique<Bind::VertexShader>("shaders//" + VS + ".shd");
			ID3DBlob* pVSByteCode = pVS->GetByteCode();
			pBinds[Bind::idVertexShader] = std::move(pVS);
			pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);
			//pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>("shaders//" + PS + ".shd");*/
			pBinds[Bind::idPixelShader] = std::make_unique<Bind::Null_PixelShader>();

			//m_FXBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<MaterialPSConstBuffer>>(matCBuffer, 0u);

			// create step 0
			m_Steps.push_back(RTechniqueStep{ std::move(pBinds), 1u });
		}

		//
		// STEP FOR PASS 2
		//

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

			// create step 0
			m_Steps.push_back(RTechniqueStep{ std::move(pBinds), 2u });
		}

		break;
	}
	default:
	{
		{
			// since it's a Standard pass - create step with null bound vector
			//fInitBinds();
			//m_Steps.push_back(RTechniqueStep{std::move(pBinds), 0u});
		}
		break;
	}
	}
}

const uint32_t& RTechnique::GetId() const noexcept
{
	return m_id;
}

std::vector<RTechniqueStep>* RTechnique::Steps() noexcept
{
	return &m_Steps;
}
