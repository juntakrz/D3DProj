#pragma once

#include "D3DMgr/Meshes/Mesh_Includes.h"
#include "DFactory.h"

using namespace DirectX;

class RenderGraph;

struct DFMesh
{
	uint32_t meshid;
	std::string meshName;
	std::string meshMat;
	std::unique_ptr<MeshCore> pMesh;	// real mesh
	std::unique_ptr<MeshCore> pOTMesh;	// occlusion testing mesh
};

// each node stores ptr to meshes and child nodes that will all be affected by its own transform
class DFModelNode
{
private:
	friend class DFModelMgr;

	std::vector<std::unique_ptr<DFModelNode>> m_pChildNodes;
	std::vector<MeshCore*> m_pMeshes;
	std::vector<MeshCore*> m_pAABBs;
	mutable XMMATRIX m_nodeTransform, m_accTransform;

	void AddChildNode(std::unique_ptr<DFModelNode> pChild) noexcept;

public:
	DFModelNode(std::vector<MeshCore*> pMeshes, const XMMATRIX& transform) noexcept;
	DFModelNode(std::vector<MeshCore*> pMeshes, std::vector<MeshCore*> pAABBs, const XMMATRIX& transform) noexcept;
	void XMUpdate(FXMMATRIX transform) noexcept;
	void CreateRenderJob(RenderGraph* renderMgr) const noexcept;
};

class DFModelMgr
{
	friend class DFactory;
	friend class DFModelNode;

private:
	uint32_t m_curModel = 0;

	struct
	{
		bool storedT = false;
		bool storedS = false;
		bool storedR = false;
		bool CBX_UniformS = true;
		XMFLOAT3A tempTranslation;
		XMFLOAT3A tempRotation;
		XMFLOAT3A tempScaling;
	} DFModelM_UIData;

	struct DFModel
	{
		uint32_t id;
		std::string name;
		std::unique_ptr<DFModelNode> pRootNode;
		std::vector<DFMesh> meshes;

		bool m_followCamera = false;

		struct
		{
			XMFLOAT3A translation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A rotation = { 0.0f, 0.0f, 0.0f };
			XMFLOAT3A scaling = { 1.0f, 1.0f, 1.0f };
		} transform;

		XMMATRIX xmMain = XMMatrixIdentity();
		FXMMATRIX GetModelXMTransform() noexcept;

	public:
		void SetPos(const XMFLOAT3A& position) noexcept;
		void SetPos(const float& x, const float& y, const float& z) noexcept;

		void AdjustPos(const XMFLOAT3A& position) noexcept;
		void AdjustPos(const float& x, const float& y, const float& z) noexcept;

		void FollowCamera(const bool& enabled) noexcept;	// add camera position to model translation
	};

	D3DMgr* pD3DMgr;
	DFMaterial* pMatMgr;
	RenderGraph* pRenderMgr;

	std::vector<DFModel> m_Models;

	// ASSIMP
	DFMesh ParseAIMesh(const aiMesh& mesh, aiMaterial** const ppMaterials, uint32_t meshID, const bool& createAABB);
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

	void Create(uint8_t type, std::string name, const bool& createAABB = true, uint16_t paramA = 0, uint16_t paramB = 0) noexcept;
	void Create(uint8_t type, std::string name, const bool& createAABB, std::string path) noexcept;

	// select current model
	DFModel& Select(std::string name) noexcept;		// searches, selects and returns model by name (slow)
	DFModel& Select(uint32_t modelID) noexcept;		// searches, selects and returns model by Id (faster)
	DFModel& Model() noexcept;						// returns selected model (very fast)

	void Delete() noexcept;
	void Delete(std::string name) noexcept;
	void Delete(uint32_t modelID) noexcept;

	void UpdateRenderer() noexcept;

	void SetMaterial(std::string material = "Mat_Default", uint32_t meshID = 0) noexcept;
	void SetMaterialRT(std::string material = "Mat_RTTDefault", uint32_t meshID = 0) noexcept;
	void SetShaders(std::string VS = "", std::string PS = "", uint32_t meshID = 0) noexcept;
	void SetEffect(uint32_t techniqueIds = DF::Pass::Standard, uint32_t meshID = 0) noexcept;

	void DEBUG_ShowModelList() const noexcept;

	void SetPos(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetRotation(float x = 0.0f, float y = 0.0f, float z = 0.0f) noexcept;
	void SetScaling(float x = 1.0f, float y = 1.0f, float z = 1.0f) noexcept;

	void ShowEditWindow() noexcept;
};