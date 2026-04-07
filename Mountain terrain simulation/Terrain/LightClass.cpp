#include "LightClass.h"

LightClass::LightClass()
{
	m_ambientColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_lookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = XMFLOAT4(red, green, blue, alpha);

	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);

	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = XMFLOAT3(x, y, z);

	return;
}

void LightClass::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);

	return;
}

XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}

XMFLOAT3 LightClass::GetPosition()
{
	return m_position;
}

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = XMFLOAT3(x, y, z);
}

void LightClass::GenerateViewMatrix()
{
	XMFLOAT3 up;
	XMVECTOR positionVec, lookAtVec, upVec;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	positionVec = XMLoadFloat3(&m_position);
	lookAtVec = XMLoadFloat3(&m_lookAt);
	upVec = XMLoadFloat3(&up);

	m_viewMatrix = XMMatrixLookAtLH(positionVec, lookAtVec, upVec);
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float orthoWidth = 1024.0f;
	float orthoHeight = 1024.0f;

	m_projectionMatrix = XMMatrixOrthographicLH(orthoWidth, orthoHeight, screenNear, screenDepth);
}

void LightClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void LightClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}
