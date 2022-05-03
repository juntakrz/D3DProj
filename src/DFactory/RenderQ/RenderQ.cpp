#include "RenderQ.h"

RenderQ::RenderQ() noexcept
{
	// create preset passes
	PassCreate("fxStandard0");		// 0 = fxStandard
	PassCreate("fxOutline1");		// 1 = fxOutline / Write
	PassCreate("fxOutline2");		// 2 = fxOutline / Mask

	// create techniques from presets
	TechniqueCreate(DF::fxOutline);	// 1 << 1 = fxOutline

	// initialize static pointers to engine methods in Pass class
	RPass::Init();
}

void RenderQ::Render() noexcept
{
	for (const auto& it : m_Passes)
	{
		it.PassDraw();
	}
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

void RenderQ::TechniqueCreate(uint32_t id) noexcept
{
	for (const auto& it : m_Techniques)
	{
		if (it->GetId() == id)
		{
#ifdef _DEBUG || _DFDEBUG
			std::stringstream sstr;
			sstr << "RenderQ Error: Technique with ID: " << id << " already exists.\n";
			OutputDebugStringA(std::string(sstr.str()).c_str());
#endif
		}
	}
	m_Techniques.emplace_back(std::make_unique<RTechnique>(id));
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
	if (!techniqueIds)	// if 0 - ignore ane don't render
	{
		return;
	}

	// if technique is 1 ('standard') binds will be taken from the mesh itself
	(techniqueIds & 1) ? m_Passes[0].PassJobAdd(RPassJob{ pMesh, nullptr }) : void();

	// iterate through technique IDs of a mech and generate job
	// techniques should be placed in a vector in the same order as they are in bit flags
	for (uint8_t i = 1; i < 4; i++)
	{
		if (techniqueIds >> i & 1)
		{
			// get technique by id and iterate through its steps
			for (auto& it : *m_Techniques[i]->Steps())
			{
				// add render job at pass defined in technique step
				// job: a ptr to mesh and a ptr to step which will contain technique specific binds
				m_Passes[it.m_PassId].PassJobAdd(RPassJob{pMesh, &it});
			}
		}
	}
}
