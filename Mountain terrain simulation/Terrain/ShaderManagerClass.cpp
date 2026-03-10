#include "ShaderManagerClass.h"

ShaderManagerClass::ShaderManagerClass()
{
	m_ColorShader = NULL;
	m_TextureShader = NULL;
	m_LightShader = NULL;
	m_FontShader = NULL;
	m_SkyDomeShader = NULL;
	m_TerrainShader = NULL;
	m_SkyboxShader = NULL;  
}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	m_ColorShader = new ColorShaderClass();
	m_TextureShader = new TextureShaderClass();
	m_LightShader = new LightShaderClass();
	m_FontShader = new FontShaderClass();
	m_SkyDomeShader = new SkyDomeShaderClass();
	m_TerrainShader = new TerrainShaderClass();
	m_SkyboxShader = new SkyboxShaderClass(); 

	if (!m_ColorShader ||
		!m_TextureShader ||
		!m_LightShader ||
		!m_FontShader ||
		!m_SkyDomeShader ||
		!m_TerrainShader ||
		!m_SkyboxShader || 
		!m_ColorShader->Initialize(device, hwnd) ||
		!m_TextureShader->Initialize(device, hwnd) ||
		!m_LightShader->Initialize(device, hwnd) ||
		!m_FontShader->Initialize(device, hwnd) ||
		!m_SkyDomeShader->Initialize(device, hwnd) ||
		!m_TerrainShader->Initialize(device, hwnd) ||
		!m_SkyboxShader->Initialize(device, hwnd) 
		) {
		return false;
	}

	return true;
}

void ShaderManagerClass::Shutdown()
{
	if (m_SkyboxShader) {
		m_SkyboxShader->Shutdown();
		delete m_SkyboxShader;
		m_SkyboxShader = NULL;
	}

	if (m_TerrainShader) {
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = NULL;
	}

	if (m_SkyDomeShader) {
		m_SkyDomeShader->Shutdown();
		delete m_SkyDomeShader;
		m_SkyDomeShader = NULL;
	}

	if (m_FontShader) {
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = NULL;
	}

	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = NULL;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = NULL;
	}

	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = NULL;
	}
}

bool ShaderManagerClass::RenderColorShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatix,
	XMMATRIX projectionMatrix)
{
	return m_ColorShader->Render(
		deviceContext,
		indexCount,
		worldMatrix,
		viewMatix,
		projectionMatrix
	);
}

bool ShaderManagerClass::RenderTextureShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	return m_TextureShader->Render(
		deviceContext,
		indexCount,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture
	);
}

bool ShaderManagerClass::RenderLightShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection,
	XMFLOAT4 diffuseColor)
{
	return m_LightShader->Render(
		deviceContext,
		indexCount,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture,
		lightDirection,
		diffuseColor
	);
}

bool ShaderManagerClass::RenderFontShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture,
	XMFLOAT4 color)
{
	return m_FontShader->Render(
		deviceContext,
		indexCount,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		texture,
		color
	);
}

bool ShaderManagerClass::RenderSkyDomeShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	XMFLOAT4 apexColor,
	XMFLOAT4 centerColor)
{
	return m_SkyDomeShader->Render(
		deviceContext,
		indexCount,
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		apexColor,
		centerColor
	);
}

bool ShaderManagerClass::RenderTerrainShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture,
	ID3D11ShaderResourceView* normalMap,
	ID3D11ShaderResourceView* normalMap2,
	ID3D11ShaderResourceView* normalMap3,
	ID3D11ShaderResourceView* grassTexture,     // NEW
	ID3D11ShaderResourceView* grassNormalMap,   // NEW
	XMFLOAT3 lightDirection,
	XMFLOAT4 diffuseColor)
{
	return m_TerrainShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix,
		texture, normalMap, normalMap2, normalMap3, grassTexture, grassNormalMap,
		lightDirection, diffuseColor);
}

// NEW: RenderSkyboxShader
bool ShaderManagerClass::RenderSkyboxShader(
	ID3D11DeviceContext* deviceContext,
	int indexCount,
	XMMATRIX worldMatrix,
	XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* cubemapTexture)
{
	return m_SkyboxShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, cubemapTexture);
}