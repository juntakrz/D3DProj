#include "RenderQ.h"

RenderQ::RenderQ() noexcept
{
	// create preset passes
	PassCreate("fxStandard0");			// 0 = fxStandard
	PassCreate("fxBlur0");				// 1 = fxBlur
	PassCreate("fxOutline0");			// 2 = fxOutline (step 0)
	PassCreate("fxOutline1");			// 3 = fxOutline (step 1)

	// initialize default techniques
	RTechniqueDB::Get().InitDefaultTechniques();

	// initialize static pointers to engine methods in Pass class
	RPass::Init();
}

void RenderQ::Render() noexcept
{
	// render scenario // need to rewrite this

	DF::DFM->SetDepthStencilState(0);	// stOff
	m_Passes[0].PassDraw();				// fxStandard

	DF::DFM->RTBind(2u, 1u);			// bind 'blur buffer' for rendering but write depth to 'render' depth buffer
	m_Passes[1].PassDraw();				// fxBlur drawing to blur buffer and render stencil pass

	DF::DFM->SetDepthStencilState(1);	// stWrite
	m_Passes[2].PassDraw();				// fxOutline stencil writing step

	DF::DFM->SetDepthStencilState(2);	// stMask
	m_Passes[3].PassDraw();				// fxOutline stencil masking step

	DF::DFM->SetDepthStencilState(0);	// stOff
}

void RenderQ::PassCreate(std::string name) noexcept
{
	for (const auto& it : m_Passes)
	{
		if (it.m_Name == name)
		{
#ifdef _DEBUG || _DFDEBUG
			name = "RenderQ Error: Pass '" + name + "' already exists.";
			OutputDebugStringA(name.c_str());
#endif
			return;
		};
	}
	m_Passes.emplace_back(RPass{ m_Passes.size(), name });
}

RPass& RenderQ::Pass(std::string name) noexcept
{
	for (auto& it : m_Passes)
	{
		if (it.m_Name == name)
		{
			return it;
		}
	}

#ifdef _DEBUG || _DFDEBUG
	name = "RenderQ: Pass " + name + " not found. Returning Pass 0.\n";
	OutputDebugStringA(name.c_str());
#endif

	return m_Passes[0];
}

void RenderQ::ResetRenderPasses() noexcept
{
	for (auto& it : m_Passes)
	{
		it.m_Jobs.clear();
	}
}

void RenderQ::GenerateJob(MeshCore* pMesh, uint32_t techniqueIds) noexcept
{
	if (!techniqueIds)	// if 0 - ignore and don't render
	{
		return;
	}
	
	for (uint8_t i = 0; i < RTechniqueDB::Get().Size(); i++)
	{
		// add render job at pass equal to technique index
		// job: a ptr to mesh, binds will be received from technique with the same index as this pass
		(techniqueIds >> i & 1) ? m_Passes[i].PassJobAdd(RPassJob{ pMesh }) : void();
	}
}
