#include "MinimapClass.h"

MiniMapClass::MiniMapClass()
{
	m_MiniMapBitmap = 0;
	m_PointBitmap = 0;
	m_mapLocationX;
	m_mapLocationY;
	m_mapSizeX;
	m_mapSizeY;
	m_pointLocationX;
	m_pointLocationY;
	m_terrainHeight;
	m_terrainWidth;
}

MiniMapClass::~MiniMapClass()
{
}

bool MiniMapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight, float terrainWidth, float terrainHeight)
{
	m_mapSizeX = 150.0f;
	m_mapSizeY = 150.0f;

	m_mapLocationX = screenWidth - (int)m_mapSizeX - 10;
	m_mapLocationY = 10;

	m_terrainWidth = terrainWidth;
	m_terrainHeight = terrainHeight;

	m_MiniMapBitmap = new BitmapClass();
	m_PointBitmap = new BitmapClass();

	if (!m_MiniMapBitmap ||
		!m_PointBitmap ||
		!m_MiniMapBitmap->Initialize(device, deviceContext, screenWidth, screenHeight, 154, 154, "data/minimap/minimap.tga") ||
		!m_PointBitmap->Initialize(device, deviceContext, screenWidth, screenHeight, 3, 3, "data/minimap/point.tga")
	) {
		return false;
	}

	return true;
}

void MiniMapClass::Shutdown()
{
	if (m_PointBitmap) {
		m_PointBitmap->Shutdown();
		delete m_PointBitmap;
		m_PointBitmap = NULL;
	}

	if (m_MiniMapBitmap) {
		m_MiniMapBitmap->Shutdown();
		delete m_MiniMapBitmap;
		m_MiniMapBitmap = NULL;
	}

	return;
}

bool MiniMapClass::Render(ID3D11DeviceContext* deviceContext, ShaderManagerClass* shaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	if (!m_MiniMapBitmap->Render(deviceContext, m_mapLocationX, m_mapLocationY) ||
		!shaderManager->RenderTextureShader(deviceContext, m_MiniMapBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_MiniMapBitmap->GetTexture()) ||
		!m_PointBitmap->Render(deviceContext, m_pointLocationX, m_pointLocationY) ||
		!shaderManager->RenderTextureShader(deviceContext, m_PointBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_PointBitmap->GetTexture())
	) {
		return false;
	}

	return true;
}

void MiniMapClass::PositionUpdate(float positionX, float positionZ)
{
	float percentX, percentY;

	if (positionX < 0) {
		positionX = 0.0f;
	}

	if (positionZ < 0) {
		positionZ = 0.0f;
	}

	if (positionX > m_terrainWidth) {
		positionX = m_terrainWidth;
	}

	if (positionZ > m_terrainHeight) {
		positionZ = m_terrainHeight;
	}

	percentX = positionX / m_terrainWidth;
	percentY = 1.0f - positionZ / m_terrainHeight;

	m_pointLocationX = (m_mapLocationX + 2) + (int)(percentX * m_mapSizeX);
	m_pointLocationY = (m_mapLocationY + 2) + (int)(percentY * m_mapSizeY);

	m_pointLocationX -= 1.0f;
	m_pointLocationY -= 1.0f;

	return;
}
