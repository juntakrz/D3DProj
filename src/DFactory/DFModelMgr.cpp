#include "DFModelMgr.h"

// DF MODEL MANAGER

DFModel* DFModelMgr::Create(uint8_t type, std::string name, const bool& createAABB, uint16_t paramA, uint16_t paramB) noexcept
{
	DFModel newModel;
	DFMesh newMesh;
	std::vector<MeshCore*> pMeshes;
	std::vector<MeshCore*> pAABBs;
	uint32_t newID = 0;

	for (auto& it : m_Models)
	{
		if (it.name == name) return &it;

		if (newID == it.id)
		{
			newID++;
		}
		else
		{
			break;
		}
	}

	switch (type)
	{
	case DF::idPlane:
	{
		paramA < 1 ? paramA = 1 : paramA;
		paramB < 1 ? paramB = 1 : paramB;
		newMesh.meshid = 1;
		newMesh.meshMat = DF::Default::material;
		newMesh.meshName = "Plane" + std::to_string(newID);
		newMesh.pMesh = std::make_unique<MeshPlane>(DF::Default::material.c_str(), paramA, paramB);

		// generate AABB if required
		newMesh.pOTMesh = (createAABB) ? std::make_unique<MeshAABB>(newMesh.pMesh->AABBPoints()) : nullptr;
		newMesh.pMesh->m_isPredicateEnabled = createAABB;
		newMesh.pMesh->m_hasOwnAABB = createAABB;

		newModel.id = newID;
		newModel.name = name;
		newModel.meshes.push_back(std::move(newMesh));

		pMeshes.push_back(newModel.meshes[0].pMesh.get());
		(createAABB) ? pAABBs.push_back(newModel.meshes[0].pOTMesh.get()) : void();

		m_Models.emplace_back(std::move(newModel));
		
		// if AABB is present - add it to node
		m_Models.back().pRootNode = (createAABB)
			? std::make_unique<DFModelNode>(std::move(pMeshes), std::move(pAABBs), XMMatrixIdentity())
			: std::make_unique<DFModelNode>(std::move(pMeshes), XMMatrixIdentity());
		m_curModel = m_Models.size() - 1;

		break;
	}
	case DF::idCube:
	{
		newMesh.meshid = 1;
		newMesh.meshMat = DF::Default::material;
		newMesh.meshName = "Cube" + std::to_string(newID);
		newMesh.pMesh = std::make_unique<MeshCube>(DF::Default::material.c_str());

		// generate AABB if required
		newMesh.pOTMesh = (createAABB) ? std::make_unique<MeshAABB>(newMesh.pMesh->AABBPoints()) : nullptr;
		newMesh.pMesh->m_isPredicateEnabled = createAABB;
		newMesh.pMesh->m_hasOwnAABB = createAABB;

		newModel.id = newID;
		newModel.name = name;
		newModel.meshes.push_back(std::move(newMesh));

		pMeshes.push_back(newModel.meshes[0].pMesh.get());
		(createAABB) ? pAABBs.push_back(newModel.meshes[0].pOTMesh.get()) : void();

		m_Models.emplace_back(std::move(newModel));
		// if AABB is present - add it to node
		m_Models.back().pRootNode = (createAABB)
			? std::make_unique<DFModelNode>(std::move(pMeshes), std::move(pAABBs), XMMatrixIdentity())
			: std::make_unique<DFModelNode>(std::move(pMeshes), XMMatrixIdentity());
		m_curModel = m_Models.size() - 1;

		break;
	}
	case DF::idSphere:
	{
		(paramA < 1) ? paramA = 32 : paramA;
		
		newMesh.meshid = 1;
		newMesh.meshMat = DF::Default::material;
		newMesh.meshName = "Sphere" + std::to_string(newID);
		newMesh.pMesh = std::make_unique<MeshSphere>(DF::Default::material.c_str(), paramA);

		// generate AABB if required
		newMesh.pOTMesh = (createAABB) ? std::make_unique<MeshAABB>(newMesh.pMesh->AABBPoints()) : nullptr;
		newMesh.pMesh->m_isPredicateEnabled = createAABB;
		newMesh.pMesh->m_hasOwnAABB = createAABB;

		newModel.id = newID;
		newModel.name = name;
		newModel.meshes.push_back(std::move(newMesh));

		pMeshes.push_back(newModel.meshes[0].pMesh.get());
		(createAABB) ? pAABBs.push_back(newModel.meshes[0].pOTMesh.get()) : void();

		m_Models.emplace_back(std::move(newModel));
		// if AABB is present - add it to node
		m_Models.back().pRootNode = (createAABB)
			? std::make_unique<DFModelNode>(std::move(pMeshes), std::move(pAABBs), XMMatrixIdentity())
			: std::make_unique<DFModelNode>(std::move(pMeshes), XMMatrixIdentity());
		m_curModel = m_Models.size() - 1;
		
		break;
	}
	case DF::idSkySphere:
	{
		// skysphere does not require any bounding meshes as it shouldn't be occlusion tested

		(paramA < 1) ? paramA = 12 : paramA;

		newMesh.meshid = 1;
		newMesh.meshMat = DF::Default::material;
		newMesh.meshName = "SkySphere" + std::to_string(newMesh.meshid);
		newMesh.pMesh = std::make_unique<MeshSphere>(DF::Default::material.c_str(), paramA, true);

		newModel.id = newID;
		newModel.name = name;
		newModel.meshes.push_back(std::move(newMesh));

		pMeshes.push_back(newModel.meshes[0].pMesh.get());

		m_Models.emplace_back(std::move(newModel));
		m_Models.back().pRootNode = std::make_unique<DFModelNode>(std::move(pMeshes), XMMatrixIdentity());
		m_curModel = m_Models.size() - 1;

		break;
	}
	case DF::idPoint:
	{
		newMesh.meshid = 1;
		newMesh.meshMat = DF::Default::material;
		newMesh.meshName = "Point" + std::to_string(newMesh.meshid);
		newMesh.pMesh = std::make_unique<MeshPoint>(DF::Default::material.c_str());

		newModel.id = newID;
		newModel.name = name;
		newModel.meshes.push_back(std::move(newMesh));

		pMeshes.push_back(newModel.meshes[0].pMesh.get());

		m_Models.emplace_back(std::move(newModel));
		m_Models.back().pRootNode = std::make_unique<DFModelNode>(std::move(pMeshes), XMMatrixIdentity());
		m_curModel = m_Models.size() - 1;

		break;
	}
	}
	m_curModel = newID;

	return &m_Models.back();
}

DFModel& DFModelMgr::Select(std::string name) noexcept
{
	uint32_t index = 0;
	for (auto& it : m_Models)
	{
		if (it.name == name)
		{
			m_curModel = index;
			return it;
		}
		index++;
	}
	return m_Models[m_curModel];
}

DFModel& DFModelMgr::Select(uint32_t modelID) noexcept
{
	uint32_t index = 0;
	for (auto& it : m_Models)
	{
		if (it.id == modelID)
		{
			m_curModel = index;
			return it;
		}
		index++;
	}

	if (m_Models.empty())
	{
		MessageBoxA(nullptr, "No models were loaded.", "Model Manager Error", MB_OK | MB_ICONERROR);
	}
	return m_Models[m_curModel];
}

DFModel& DFModelMgr::Model() noexcept
{
	return m_Models[m_curModel];
}

DFModel* DFModelMgr::Find(const char* id) noexcept
{
	for (auto& it : m_Models)
	{
		if (it.name == id)
		{
			return &it;
		}
	}

	return nullptr;
}

void DFModelMgr::Delete() noexcept
{
	m_Models.erase(m_Models.begin() + m_curModel);
	m_Models.shrink_to_fit();
}

void DFModelMgr::UpdateRenderer() noexcept
{
	pRenderMgr->ResetJobs();

	for (const auto& it : m_Models)
	{
		// process meshes and fill the render queue
		it.pRootNode->CreateRenderJob(pRenderMgr);
	}
}

void DFModelMgr::SetMaterial(std::string material, uint32_t meshID) noexcept
{
	// if ID is 0 - set material for all meshes
	if (meshID == 0)
	{
		for (auto& it : m_Models[m_curModel].meshes)
		{
			it.pMesh->SetMaterial(material);
			it.meshMat = material;
		}
	}
	else
	{
		m_Models[m_curModel].meshes[meshID].pMesh->SetMaterial(material);
		m_Models[m_curModel].meshes[meshID].meshMat = material;
	}
}

void DFModelMgr::SetMaterialRT(std::string material, uint32_t meshID) noexcept
{
	// if ID is 0 - set material for all meshes
	if (meshID == 0)
	{
		for (auto& it : m_Models[m_curModel].meshes)
		{
			it.pMesh->SetMaterialRT(material);
			it.meshMat = material;
		}
	}
	else
	{
		m_Models[m_curModel].meshes[meshID].pMesh->SetMaterialRT(material);
		m_Models[m_curModel].meshes[meshID].meshMat = material;
	}
}

void DFModelMgr::SetShaders(std::string VS, std::string PS, uint32_t meshID) noexcept
{
	// if ID is 0 - set material for all meshes
	if (meshID == 0)
	{
		for (auto& it : m_Models[m_curModel].meshes)
		{
			it.pMesh->SetShaders(VS, PS);
		}
	}
	else
	{
		m_Models[m_curModel].meshes[meshID].pMesh->SetShaders(VS, PS);
	}
}

void DFModelMgr::SetEffect(uint32_t techniqueIds, uint32_t meshID) noexcept
{
	// if ID is 0 - set effect for all meshes
	if (meshID == 0)
	{
		for (auto& it : m_Models[m_curModel].meshes)
		{
			it.pMesh->SetEffects(techniqueIds);
		}
	}
	else
	{
		m_Models[m_curModel].meshes[meshID].pMesh->SetEffects(techniqueIds);
	}
}

void DFModelMgr::DEBUG_ShowModelList() const noexcept
{
	std::stringstream sstr;
	for (const auto& it : m_Models)
	{
		sstr << "[Model #" << it.id << ": " << it.name << "]\n";
		for (const auto& mesh : it.meshes)
		{
			sstr << "\tMesh " << mesh.meshid << ": " << mesh.meshName << " (" << mesh.meshMat << ")\n";
		}
		sstr << "\n";
	}

	MessageBoxA(nullptr, std::string(sstr.str()).c_str(), "DEBUG: Model Manager", MB_OK | MB_ICONINFORMATION);
}