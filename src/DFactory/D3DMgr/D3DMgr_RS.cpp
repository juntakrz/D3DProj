#include "D3DMgr.h"

uint8_t D3DMgr::CreateRenderSurface(std::string name, float scale) noexcept
{
	// create new surface target entry
	surfaceTargets.push_back(SurfaceTarget{});
	surfaceTargets.back().name = name;
	surfaceTargets.back().pRS = std::make_unique<RenderSurface>(scale);

	return surfaceTargets.size() - 1;
}

std::vector<D3DMgr::SurfaceTarget>* D3DMgr::SurfaceTargets() noexcept
{
	return &surfaceTargets;
}

RenderSurface* D3DMgr::SurfaceTargets(uint8_t index) noexcept
{
	(index > surfaceTargets.size() - 1) ?
		MessageBoxA(nullptr, "Render surface index is out of bounds.\n", "D3D Manager Error", MB_OK | MB_ICONERROR) : 0;

	return surfaceTargets[index].pRS.get();
}

