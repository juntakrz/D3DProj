#pragma once

namespace Bind
{
	enum BindID
	{
		idIndexBuffer, idVertexBuffer, idSampler, idInputLayout, idTransform,
		idVertexShader, idPixelShader,
		idTexture0, idTexture1, idTexture2, idTexture3, idTexture4, idTexture5,
		idConstVertexBuf0, idConstPixelBuf0,
		idTopology
	};
}

namespace DF
{
	enum ObjectID
	{
		idPlane = 0, idCube, idSphere, idSkySphere, idImport
	};
}