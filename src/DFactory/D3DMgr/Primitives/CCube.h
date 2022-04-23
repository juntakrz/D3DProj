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
		std::vector<uint32_t> indices(24);

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
};