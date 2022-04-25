#include "DFModelMgr.h"

void DFModelMgr::Create(uint8_t type, std::string name, std::string path) noexcept
{
	uint32_t newID = 0;

	for (const auto& it : m_Models)
	{
		if (it.name == name) return;
	}

	for (const auto& it : m_Models)
	{
		if (newID == it.id)
		{
			newID++;
		}
		else
		{
			break;
		}
	}

	if (type == DF::idImport)
	{
		path = "meshes//" + path;
		Assimp::Importer importer;
		const aiScene* pScene = importer.ReadFile(path.c_str(),
			aiProcess_Triangulate
			//| aiProcess_JoinIdenticalVertices
			| aiProcess_ConvertToLeftHanded
			| aiProcess_ImproveCacheLocality
			| aiProcess_GenSmoothNormals
			//| aiProcess_OptimizeMeshes
			//| aiProcess_OptimizeGraph
			| aiProcess_CalcTangentSpace
		);

		DFModel newModel;

		for (uint32_t i = 0; i < pScene->mNumMeshes; i++)
		{
			newModel.meshes.emplace_back(std::move(ParseAIMesh(*pScene->mMeshes[i], pScene->mMaterials, i + 1)));
		}

		newModel.pRootNode = ParseAINode(*pScene->mRootNode, &newModel.meshes);
		newModel.id = newID;
		newModel.name = name;

		m_Models.emplace_back(std::move(newModel));
		m_curModel = newID;
	}
}

DFMesh DFModelMgr::ParseAIMesh(const aiMesh& mesh, aiMaterial** const ppMaterials, uint32_t meshID)
{
	DFMesh newMesh;

	newMesh.meshid = meshID;

	mesh.mName.C_Str() ? newMesh.meshName = mesh.mName.C_Str() : newMesh.meshName = "AIMesh" + std::to_string(meshID);

	// creating model input layout
	Vrtx::Buffer vbuf(std::move(Vrtx::Layout{}
		.Append(Vrtx::Layout::Position3D)
		.Append(Vrtx::Layout::Texture2D)
		.Append(Vrtx::Layout::Normal)
		.Append(Vrtx::Layout::Tangent)
		.Append(Vrtx::Layout::Binormal)
	));

	for (uint32_t i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i])
		);
	}

	std::vector<uint32_t> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (uint32_t j = 0; j < mesh.mNumFaces; j++)
	{
		//ASSERT(mesh.mFaces->mNumIndices == 3);

		indices.push_back(mesh.mFaces[j].mIndices[0]);
		indices.push_back(mesh.mFaces[j].mIndices[1]);
		indices.push_back(mesh.mFaces[j].mIndices[2]);
	}

	std::vector<std::unique_ptr<Bind::IBind>> pBinds;
	pBinds.reserve(24);
	for (uint8_t i = 0; i < 24; i++)
	{
		pBinds.emplace_back();
	}

	if (mesh.mMaterialIndex >= 0)
	{
		aiMaterial* pMaterial = ppMaterials[mesh.mMaterialIndex];
		aiString texColorName, texNormalName, texSpecularName;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texColorName);
			std::string texColorPath(texColorName.C_Str());
			newMesh.meshMat = texColorName.C_Str();
			pBinds[Bind::idTexture0] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture(texColorPath)), 0u);
		}
		else	//if imported model has no color textures = use the default checkerboard one
		{
			pBinds[Bind::idTexture0] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture("default//default.png")), 0u);
			newMesh.meshMat = "Mat_Default";
		}

		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texNormalName);
			std::string texNormalPath(texNormalName.C_Str());
			pBinds[Bind::idTexture1] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture(texNormalPath)), 1u);
		}
		else
		{
			pBinds[Bind::idTexture1] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture("default//default_n.png")), 1u);
		}

		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texSpecularName);
			std::string texSpecularPath(texSpecularName.C_Str());
			pBinds[Bind::idTexture2] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture(texSpecularPath)), 2u);
		}
		else
		{
			pBinds[Bind::idTexture2] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture("default//default_s.png")), 2u);
		}

		/*
		if (pMaterial->GetTextureCount(aiTextureType_REFLECTION) > 0)
		{
			pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texSpecularName);
			std::string texSpecularPath(texSpecularName.C_Str());
			pBinds[Bind::idTexture3] = std::make_unique<Bind::Texture>(*pD3DMgr, *pMatMgr->GetTexture(pMatMgr->AddTexture(texSpecularPath)), 3u);
		}*/
	}

	pBinds[Bind::idSampler] = std::make_unique<Bind::Sampler>(*pD3DMgr);

	pBinds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(*pD3DMgr, vbuf);
	pBinds[Bind::idIndexBuffer] = std::make_unique<Bind::IndexBuffer>(*pD3DMgr, indices);

	//create and bind vertex shader
	std::string VSPath = "shaders//VS_BasicTexture.cso";
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(*pD3DMgr, VSPath);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	pBinds[Bind::idVertexShader] = std::move(pVS);

	//create and bind pixel shader
	std::string PSPath = "shaders//PS_BasicTexture.cso";
	pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(*pD3DMgr, PSPath);

	pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(*pD3DMgr, vbuf.GetLayout().GetInputLayoutDesc(), pVSByteCode);

	struct PSConstBuffer
	{
		XMFLOAT4 ambientColor = { 0.0f, 0.0f, 0.16f, 0.0f };
		float matIntensity = 1.0f;
		float specIntensity = 3.0f;
		float specPower = 320.0f;
		float _padding;
	} material;

	pBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<PSConstBuffer>>(*pD3DMgr, material, 0u);

	newMesh.pMesh = std::make_unique<MeshImport>(pD3DMgr, std::move(pBinds));

	return newMesh;
}

std::unique_ptr<DFModelNode> DFModelMgr::ParseAINode(const aiNode& node, const std::vector<DFMesh>* pDFMeshes)
{
	std::vector<MeshCore*> pMeshes;

	pMeshes.reserve(node.mNumMeshes);

	// get transformation matrix of current node
	XMMATRIX nodeTransform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)));

	// get IDs for meshes of a current node and add corresponding meshes from a global vector for this model
	for (uint32_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshId = node.mMeshes[i];
		pMeshes.push_back(pDFMeshes->at(meshId).pMesh.get());
	}

	// create node with unique ptr
	auto pNode = std::make_unique<DFModelNode>(std::move(pMeshes), nodeTransform);

	for (uint32_t j = 0; j < node.mNumChildren; j++)
	{
		pNode->AddChildNode(ParseAINode(*node.mChildren[j], pDFMeshes));
	}

	return pNode;
}