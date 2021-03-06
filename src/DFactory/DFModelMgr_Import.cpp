#include "DFModelMgr.h"

void DFModelMgr::Create(uint8_t type, std::string name, const bool& createAABB, std::string path) noexcept
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
			newModel.meshes.emplace_back(std::move(ParseAIMesh(*pScene->mMeshes[i], pScene->mMaterials, i + 1, createAABB)));
		}

		newModel.pRootNode = ParseAINode(*pScene->mRootNode, &newModel.meshes);
		newModel.id = newID;
		newModel.name = name;

		m_Models.emplace_back(std::move(newModel));
		m_curModel = newID;
	}
}

DFMesh DFModelMgr::ParseAIMesh(const aiMesh& mesh, aiMaterial** const ppMaterials, uint32_t meshID, const bool& createAABB)
{
	DFMesh newMesh;
	DFMaterial::Material* pDFMat = nullptr;

	newMesh.meshid = meshID;

	mesh.mName.C_Str() ? newMesh.meshName = mesh.mName.C_Str() : newMesh.meshName = "AIMesh" + std::to_string(meshID);

	// creating model input layout
	std::vector<DF::Vertex> vertices;
	for (uint32_t i = 0; i < mesh.mNumVertices; i++)
	{
		vertices.push_back({
			*reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i])
		});
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
	for (uint8_t i = 0; i < Bind::MAXBINDS; i++)
	{
		pBinds.emplace_back();
	}

	if (mesh.mMaterialIndex >= 0)
	{
		aiMaterial* pMaterial = ppMaterials[mesh.mMaterialIndex];
		aiString texName;
		std::string matName;

		//get material name and check if it exists
		matName = pMaterial->GetName().C_Str();

		if (pMatMgr->MatIndex(matName) == 0)
		{
			//create new material
			DFMaterial::DFMATERIAL_DESC DFMatDesc{};
			DFMatDesc.name = matName;
			newMesh.meshMat = matName;

			if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texName);
				DFMatDesc.textures.tex0 = texName.C_Str();
				pBinds[Bind::idTexture0] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 0u
					);
			}
			else	//if imported model has no color textures = use the default checkerboard one
			{
				pBinds[Bind::idTexture0] = std::make_unique<Bind::Texture>(pMatMgr->TextureGet("default//default.dds"), 0u);
			}

			if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texName);
				DFMatDesc.textures.tex1 = texName.C_Str();
				pBinds[Bind::idTexture1] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 1u
					);
			}
			else
			{
				DFMatDesc.textures.tex1 = "default//default_n.dds";
				pBinds[Bind::idTexture1] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd("default//default_n.dds")), 1u
					);
			}

			if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0 && pMaterial->GetTextureCount(aiTextureType_METALNESS) < 1)
			{
				pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &texName);
				DFMatDesc.textures.tex2 = texName.C_Str();
				pBinds[Bind::idTexture2] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 2u
					);
			}
			else if (pMaterial->GetTextureCount(aiTextureType_METALNESS) > 0 && pMaterial->GetTextureCount(aiTextureType_SPECULAR) < 1)
			{
				pMaterial->GetTexture(aiTextureType_METALNESS, 0, &texName);
				DFMatDesc.textures.tex2 = texName.C_Str();
				pBinds[Bind::idTexture2] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 2u
					);
			}
			else
			{
				DFMatDesc.textures.tex2 = "default//default_s.dds";
				pBinds[Bind::idTexture2] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd("default//default_s.dds")), 2u
					);
			}

			if (pMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
			{
				pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texName);
				DFMatDesc.textures.tex3 = texName.C_Str();
				pBinds[Bind::idTexture3] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 3u
					);
			}
			else
			{
				DFMatDesc.textures.tex3 = "default//default_s.dds";
				pBinds[Bind::idTexture3] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd("default//default_s.dds")), 3u
					);
			}

			if (pMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				pMaterial->GetTexture(aiTextureType_AMBIENT, 0, &texName);
				DFMatDesc.textures.tex4 = texName.C_Str();
				pBinds[Bind::idTexture4] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 4u
					);
			}
			else
			{
				DFMatDesc.textures.tex4 = "default//default_s.dds";
				pBinds[Bind::idTexture4] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd("default//default_s.dds")), 4u
					);
			}

			if (pMaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0)
			{
				pMaterial->GetTexture(aiTextureType_UNKNOWN, 0, &texName);
				DFMatDesc.textures.tex5 = texName.C_Str();
				pBinds[Bind::idTexture5] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd(texName.C_Str())), 5u
					);
			}
			else
			{
				DFMatDesc.textures.tex5 = "default//default_s.dds";
				pBinds[Bind::idTexture5] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pMatMgr->TextureAdd("default//default_s.dds")), 5u
					);
			}

			DFMatDesc.shaders.vertex = "VS_FlatTexture";
			DFMatDesc.shaders.pixel = "PS_FlatTexture";

			// default normal mapping intensity
			DFMatDesc.material.bumpIntensity = 1.5f;

			// try to delete textures if unused by anything else
			DFMatDesc.manageTextures = true;
			
			DFMatDesc.effects = DF::Pass::Standard;

			pMatMgr->MatAdd(&DFMatDesc);
			pDFMat = &pMatMgr->Mat(newMesh.meshMat);
		}
		else
		{	// use existing material
			pDFMat = &pMatMgr->Mat(matName);
			newMesh.meshMat = matName;

			for (uint16_t i = 0; i < 6; i++) {
				pBinds[Bind::idTexture0 + i] = std::make_unique<Bind::Texture>(
					pMatMgr->TextureGet(pDFMat->idTex[i]), i
					);
			}
		}
	}

	pBinds[Bind::idSampler0] = std::make_unique<Bind::Sampler>();

	pBinds[Bind::idVertexBuffer] = std::make_unique<Bind::VertexBuffer>(vertices);
	pBinds[Bind::idIndexBuffer] = std::make_unique<Bind::IndexBuffer>(indices);

	//create and bind vertex shader
	std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(pDFMat->shaderVertex);
	ID3DBlob* pVSByteCode = pVS->GetByteCode();
	pBinds[Bind::idVertexShader] = std::move(pVS);

	//create and bind pixel shader
	pBinds[Bind::idPixelShader] = std::make_unique<Bind::PixelShader>(pDFMat->shaderPixel);

	pBinds[Bind::idInputLayout] = std::make_unique<Bind::InputLayout>(DF::D3DLayout, pVSByteCode);

	struct PSConstBuffer
	{
		XMFLOAT4 ambientColor = { 0.0f, 0.0f, 0.25f, 1.0f };
		XMFLOAT3A F0 = { 0.1f, 0.1f, 0.1f };
		float matIntensity = 2.0f;
		float spec_metal = 1.12f;
		float pow_rough = 0.8f;
		float bumpIntensity = 1.5f;
	} material;

	material.bumpIntensity = pDFMat->data.w;

	pBinds[Bind::idConstPixelBuf0] = std::make_unique<Bind::ConstPixelBuffer<PSConstBuffer>>(material, 0u);

	newMesh.pMesh = std::make_unique<MeshImport>(std::move(pBinds), vertices);

	// create AABB for this mesh if required
	newMesh.pOTMesh = (createAABB) ? std::make_unique<MeshAABB>(newMesh.pMesh->AABBPoints()) : nullptr;
	newMesh.pMesh->m_isPredicateEnabled = createAABB;
	newMesh.pMesh->m_hasOwnAABB = createAABB;

	return newMesh;
}

std::unique_ptr<DFModelNode> DFModelMgr::ParseAINode(const aiNode& node, const std::vector<DFMesh>* pDFMeshes)
{
	std::vector<MeshCore*> pMeshes;
	std::vector<MeshCore*> pAABBs;

	pMeshes.reserve(node.mNumMeshes);

	// get transformation matrix of current node
	XMMATRIX nodeTransform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)));

	// get IDs for meshes of a current node and add corresponding meshes from a global vector for this model
	for (uint32_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshId = node.mMeshes[i];
		pMeshes.push_back(pDFMeshes->at(meshId).pMesh.get());
		pAABBs.push_back(pDFMeshes->at(meshId).pOTMesh.get());
	}

	// create node with unique ptr
	auto pNode = (pAABBs.size() > 0)
		? std::make_unique<DFModelNode>(std::move(pMeshes), std::move(pAABBs), nodeTransform)
		: std::make_unique<DFModelNode>(std::move(pMeshes), nodeTransform);

	for (uint32_t j = 0; j < node.mNumChildren; j++)
	{
		pNode->AddChildNode(ParseAINode(*node.mChildren[j], pDFMeshes));
	}

	return pNode;
}