#include "D3DMgr.h"

bool D3DMgr::CreateRenderSurface(std::string name, float scale) noexcept
{
	// create new surface target entry
	return surfaceTargets.try_emplace(name, std::make_unique<RenderSurface>(scale)).second;
}

RenderSurface* D3DMgr::Surface(const std::string& name) noexcept
{
	return surfaceTargets.at(name).get();
}

void D3DMgr::RenderBufferToSurface(const std::string& rtTarget, const std::string& surface) noexcept
{
	Surface(surface)->Bind(renderTargets.at(rtTarget)->pSRV.Get());
	Surface(surface)->Draw();
	Surface(surface)->Unbind();
}

void D3DMgr::RenderDepthToSurface(const std::string& dsTarget, const std::string& surface) noexcept
{
	Surface(surface)->Bind(depthTargets.at(dsTarget)->pDepthSRV.Get());
	Surface(surface)->Draw();
	Surface(surface)->Unbind();
}

