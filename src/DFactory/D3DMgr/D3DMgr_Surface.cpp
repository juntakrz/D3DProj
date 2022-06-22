#include "D3DMgr.h"

bool D3DMgr::CreateRenderSurface(const char* name, float scale) noexcept
{
	// create new surface target entry
	return surfaceTargets.try_emplace(name, std::make_unique<CRenderSurface>(scale)).second;
}

CRenderSurface* D3DMgr::Surface(const char* name) noexcept
{
	return surfaceTargets.at(name).get();
}

void D3DMgr::RenderBufferToSurface(const char* rtTarget, const char* surface) noexcept
{
	Surface(surface)->Bind(renderTargets.at(rtTarget)->pSRV.Get());
	Surface(surface)->Draw();
	Surface(surface)->Unbind();
}

void D3DMgr::RenderDepthToSurface(const char* dsTarget, const char* surface) noexcept
{
	Surface(surface)->Bind(depthTargets.at(dsTarget)->pDepthSRV.Get());
	Surface(surface)->Draw();
	Surface(surface)->Unbind();
}

void D3DMgr::RenderSurface(const char* surface) noexcept
{
	Surface(surface)->Bind();
	Surface(surface)->Draw();
}