#include "MeshCore.h"

void MeshCore::XMUpdate(FXMMATRIX transform) noexcept
{
	//receive transform from the node and apply it to the mesh transform
	//xmMain *= *transform;
	xmMain = transform;
}

DirectX::XMMATRIX* MeshCore::GetMatrix() noexcept
{
	return &xmMain;
}

const DirectX::XMMATRIX* MeshCore::GetMatrix() const noexcept
{
	return &xmMain;
}

void MeshCore::SetMatrix(DirectX::FXMMATRIX& matrix) noexcept
{
	xmMain = std::move(matrix);
}

DirectX::XMFLOAT3A* MeshCore::GetXMPos() noexcept
{
	return &xmPos;
}

void MeshCore::SetPos(float x, float y, float z) noexcept
{
	transform.translation = { x, y, z };
}

DirectX::XMFLOAT3A MeshCore::GetPos() noexcept
{
	return transform.translation;
}

void MeshCore::TranslateXYZ(float x, float y, float z) noexcept
{
	transform.translation.x += x;
	transform.translation.y += y;
	transform.translation.z += z;
}

void MeshCore::TranslateX(float x) noexcept
{
	transform.translation.x += x;
}

void MeshCore::TranslateY(float y) noexcept
{
	transform.translation.y += y;
}

void MeshCore::TranslateZ(float z) noexcept
{
	transform.translation.z += z;
}

void MeshCore::ScaleXYZ(float x, float y, float z) noexcept
{
	transform.scale.x = x;
	transform.scale.y = y;
	transform.scale.z = z;
}

void MeshCore::ScaleX(float x) noexcept
{
	transform.scale.x = x;
}

void MeshCore::ScaleY(float y) noexcept
{
	transform.scale.y = y;
}

void MeshCore::ScaleZ(float z) noexcept
{
	transform.scale.z = z;
}

void MeshCore::RotateXYZ(float x, float y, float z) noexcept
{
	transform.rotation = { x, y, -z };
}

void MeshCore::RotateX(float x) noexcept
{
	transform.rotation.x = x;
}

void MeshCore::RotateY(float y) noexcept
{
	transform.rotation.y = y;
}

void MeshCore::RotateZ(float z) noexcept
{
	transform.rotation.z = -z;
}