#pragma once

#include "EBase.h"

class DFModelNode;
struct DFMesh;

struct EModel : public EBase
{
private:
	const uint8_t m_typeId = 1;

public:
	uint32_t id;

	std::unique_ptr<DFModelNode> pRootNode;
	std::vector<DFMesh> meshes;

	bool m_followCamera = false;


public:
	void FollowCamera(const bool& enabled) noexcept;	// add camera position to model translation

	FXMMATRIX& GetXMTransform() noexcept override;
	TransformData& GetTransformData() noexcept override;
};