#pragma once

#include "CTriangleIndex.h"

class CCube
{
public:
	template<class V>
	static CTriangleIndex<V> Create()
	{
		constexpr float coord = 1.0f;

		std::vector<V> vertices(24);
		std::vector<uint32_t> indices;

		//front face
		vertices[0].pos =  { -coord, -coord, -coord };	//BL -> TL -> TR -> BR
		vertices[1].pos =  { -coord,  coord, -coord };
		vertices[2].pos =  {  coord,  coord, -coord };
		vertices[3].pos =  {  coord, -coord, -coord };

		//top face
		vertices[4].pos =  { -coord, coord, -coord };
		vertices[5].pos =  { -coord, coord,  coord };
		vertices[6].pos =  { coord,  coord,  coord };
		vertices[7].pos =  { coord,  coord, -coord };

		//left face
		vertices[8].pos =  { -coord, -coord,  coord };
		vertices[9].pos =  { -coord,  coord,  coord };
		vertices[10].pos = { -coord,  coord, -coord };
		vertices[11].pos = { -coord, -coord, -coord };

		//rear face
		vertices[12].pos = {  coord, -coord,  coord };
		vertices[13].pos = {  coord,  coord,  coord };
		vertices[14].pos = { -coord,  coord,  coord };
		vertices[15].pos = { -coord,  -coord, coord };

		//right face
		vertices[16].pos = {  coord, -coord, -coord };
		vertices[17].pos = {  coord,  coord, -coord };
		vertices[18].pos = {  coord,  coord,  coord };
		vertices[19].pos = {  coord, -coord,  coord };

		//bottom face
		vertices[20].pos = { -coord, -coord,  coord };
		vertices[21].pos = { -coord, -coord, -coord };
		vertices[22].pos = {  coord, -coord, -coord };
		vertices[23].pos = {  coord, -coord,  coord };

		for (uint8_t i = 0; i < 24; i += 4)
		{
			indices.emplace_back(i);
			indices.emplace_back(i + 1);
			indices.emplace_back(i + 2);

			indices.emplace_back(i);
			indices.emplace_back(i + 2);
			indices.emplace_back(i + 3);

			vertices[i].tex = { 0.0f, 1.0f };
			vertices[i + 1].tex = { 0.0f, 0.0f };
			vertices[i + 2].tex = { 1.0f, 0.0f };
			vertices[i + 3].tex = { 1.0f, 1.0f };
		}

		return { std::move(vertices), std::move(indices) };
	}

	// AXIS ALIGNED BOUNDING BOX
	
	template<class V>
	static CTriangleIndex<V> Create(DirectX::XMFLOAT3 vMin, DirectX::XMFLOAT3 vMax)
	{
		// increase bounding box size for safer occlusion testing
		const float bias = 0.001f;

		vMin.x -= bias;
		vMin.y -= bias;
		vMin.z -= bias;

		vMax.x += bias;
		vMax.y += bias;
		vMax.z += bias;

		std::vector<V> vertices(24);
		std::vector<uint32_t> indices;

		//front face
		vertices[0].pos =  { vMin.x, vMin.y, vMin.z };	//BL -> TL -> TR -> BR
		vertices[1].pos =  { vMin.x, vMax.y, vMin.z };
		vertices[2].pos =  { vMax.x, vMax.y, vMin.z };
		vertices[3].pos =  { vMax.x, vMin.y, vMin.z };

		//top face
		vertices[4].pos =  { vMin.x, vMax.y, vMin.z };
		vertices[5].pos =  { vMin.x, vMax.y, vMax.z };
		vertices[6].pos =  { vMax.x, vMax.y, vMax.z };
		vertices[7].pos =  { vMax.x, vMax.y, vMin.z };

		//left face
		vertices[8].pos =  { vMin.x, vMin.y, vMax.z };
		vertices[9].pos =  { vMin.x, vMax.y, vMax.z };
		vertices[10].pos = { vMin.x, vMax.y, vMin.z };
		vertices[11].pos = { vMin.x, vMin.y, vMin.z };

		//rear face
		vertices[12].pos = { vMax.x, vMin.y, vMax.z };
		vertices[13].pos = { vMax.x, vMax.y, vMax.z };
		vertices[14].pos = { vMin.x, vMax.y, vMax.z };
		vertices[15].pos = { vMin.x, vMin.y, vMax.z };

		//right face
		vertices[16].pos = { vMax.x, vMin.y, vMin.z };
		vertices[17].pos = { vMax.x, vMax.y, vMin.z };
		vertices[18].pos = { vMax.x, vMax.y, vMax.z };
		vertices[19].pos = { vMax.x, vMin.y, vMax.z };

		//bottom face
		vertices[20].pos = { vMin.x, vMin.y, vMax.z };
		vertices[21].pos = { vMin.x, vMin.y, vMin.z };
		vertices[22].pos = { vMax.x, vMin.y, vMin.z };
		vertices[23].pos = { vMax.x, vMin.y, vMax.z };

		for (uint8_t i = 0; i < 24; i += 4)
		{
			indices.emplace_back(i);
			indices.emplace_back(i + 1);
			indices.emplace_back(i + 2);

			indices.emplace_back(i);
			indices.emplace_back(i + 2);
			indices.emplace_back(i + 3);

			vertices[i].tex = { 0.0f, 1.0f };
			vertices[i + 1].tex = { 0.0f, 0.0f };
			vertices[i + 2].tex = { 1.0f, 0.0f };
			vertices[i + 3].tex = { 1.0f, 1.0f };
		}

		return { std::move(vertices), std::move(indices) };
	}

	/*
	template<class V>
	static CTriangleIndex<V> Create(DirectX::XMFLOAT3 vMin, DirectX::XMFLOAT3 vMax)
	{
		// increase bounding box size for safer occlusion testing
		const float bias = 0.001f;

		vMin.x -= bias;
		vMin.y -= bias;
		vMin.z -= bias;

		vMax.x += bias;
		vMax.y += bias;
		vMax.z += bias;

		std::vector<V> vertices(8);
		std::vector<uint32_t> indices;

		// create vertices

		//			0			1
		//		2			3
		// 
		// 
		//			4			5
		//		6			7

		vertices[0].pos = { vMin.x, vMax.y, vMax.z };
		vertices[1].pos = { vMax.x, vMax.y, vMax.z };
		vertices[2].pos = { vMin.x, vMax.y, vMin.z };
		vertices[3].pos = { vMax.x, vMax.y, vMin.z };
		vertices[4].pos = { vMin.x, vMin.y, vMax.z };
		vertices[5].pos = { vMax.x, vMin.y, vMax.z };
		vertices[6].pos = { vMin.x, vMin.y, vMin.z };
		vertices[7].pos = { vMax.x, vMin.y, vMin.z };

		// create indices

		indices =
		{
			2, 0, 1,	2, 1, 3,	// top
			6, 2, 3,	6, 3, 7,	// front
			4, 0, 2,	4, 2, 6,	// left
			7, 3, 1,	7, 1, 5,	// right
			5, 1, 0,	5, 0, 4,	// back
			4, 6, 5,	5, 6, 7		// bottom
		};

		return { std::move(vertices), std::move(indices) };
	}
	*/
};