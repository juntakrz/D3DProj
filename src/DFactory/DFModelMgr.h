#pragma once

#include "D3DMgr/Meshes/Mesh_Includes.h"
#include "DFactory.h"
#include "Util/DF_ID.h"

using namespace DirectX;

struct DFMesh
{
	uint32_t meshid;
	std::string meshName;
	std::string meshMat;
	std::unique_ptr<MeshCore> pMesh;
};

// each node stores ptr to meshes and child nodes that will all be affected by its own transform
class DFModelNode
{
private:
	friend class DFModelMgr;

	std::vector<std::unique_ptr<DFModelNode>> m_pChildNodes;
	std::vector<MeshCore*> m_pMeshes;
	mutable XMMATRIX m_nodeTransform, m_accTransform;

	void AddChildNode(std::unique_ptr<DFModelNode> pChild) noexcept;

public:
	DFModelNode(std::vector<MeshCore*> pMeshes, const XMMATRIX& transform) noexcept;
	void XMUpdate(FXMMATRIX transform) noexcept;
	void Draw() const noexcept;
};

class DFModelMgr
{
	friend class DFactory;
	friend class DFModelNode;

private:
	uint32_t m_curModel = 0;

	struct DFModel
	{
		uint32_t id;
		std::string name;
		std::unique_ptr<DFModelNode> pRootNode;
		std::vector<DFMesh> meshes;

		struct
		{
			XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A scaling = { 1.0f, 1.0f, 1.0f };
		} transform;

		XMMATRIX xmMain = XMMatrixIdentity();

		FXMMATRIX GetModelXMTransform() noexcept;
	};

	D3DMgr* pD3DMgr;
	DFMaterial* pMatMgr;

	std::vector<DFModel> m_Models;

	// ASSIMP
	DFMesh ParseAIMesh(const aiMesh& mesh, aiMaterial** const ppMaterials, uint32_t meshID);
	std::unique_ptr<DFModelNode> ParseAINode(const aiNode& node, const std::vector<DFMesh>* pDFMeshes);

public:
	DFModelMgr() = default;
	~DFModelMgr() = default;
	DFModelMgr(const DFModelMgr&) = delete;
	DFModelMgr operator=(const DFModelMgr&) = delete;
	
	static DFModelMgr& Get()
	{
		static DFModelMgr _SInstance;
		return _SInstance;
	}

	void Create(uint8_t type, std::string name, uint16_t paramA = 0, uint16_t paramB = 0) noexcept;
	void Create(uint8_t type, std::string name, std::string path) noexcept;

	// select current model
	uint32_t Select(std::string name) noexcept;
	std::string Select(uint32_t modelID) noexcept;

	void Delete() noexcept;
	void Delete(std::string name) noexcept;
	void Delete(uint32_t modelID) noexcept;

	void SetMaterial(std::string material = "Mat_Default", uint32_t meshID = 0) noexcept;
	void SetShaders(std::string VS = "", std::string PS = "", uint32_t meshID = 0) noexcept;

	void DEBUG_ShowModelList() const noexcept;

	void SetPos(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetScaling(float x = 1.0f, float y = 1.0f, float z = 1.0f) noexcept;
};