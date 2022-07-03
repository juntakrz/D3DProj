#pragma once

#include "../DFEntity.h"

class DFModelNode;
struct DFMesh;

struct DFModel : public DFEntity
{
	uint32_t id;

	std::unique_ptr<DFModelNode> pRootNode;
	std::vector<DFMesh> meshes;

	bool m_followCamera = false;


public:
	void FollowCamera(const bool& enabled) noexcept;	// add camera position to model translation

	FXMMATRIX& GetXMTransform() noexcept override;
	TransformData& GetTransformData() noexcept override;
};