#include "ShadowMapClass.h"

ShadowMapClass::ShadowMapClass()
{
	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;
	m_shaderResourceView = nullptr;
	m_shadowMapWidth = 0;
	m_shadowMapHeight = 0;
}

ShadowMapClass::~ShadowMapClass()
{
}

bool ShadowMapClass::Initialize(ID3D11Device* device, int shadowMapWidth, int shadowMapHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	m_shadowMapWidth = shadowMapWidth;
	m_shadowMapHeight = shadowMapHeight;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = shadowMapWidth;
	textureDesc.Height = shadowMapHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_depthStencilBuffer))) {
		return false;
	}

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView))) {
		return false;
	}

	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	if (FAILED(device->CreateShaderResourceView(m_depthStencilBuffer, &shaderResourceViewDesc, &m_shaderResourceView))) {
		return false;
	}

	m_viewport.Width = (float)shadowMapWidth;
	m_viewport.Height = (float)shadowMapHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	float orthoSize = 512.0f; 
	m_orthoMatrix = XMMatrixOrthographicLH(orthoSize, orthoSize, 0.1f, 1000.0f);
	m_projectionMatrix = m_orthoMatrix;

	return true;
}

void ShadowMapClass::Shutdown()
{
	if (m_shaderResourceView) {
		m_shaderResourceView->Release();
		m_shaderResourceView = nullptr;
	}

	if (m_depthStencilView) {
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilBuffer) {
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}
}

void ShadowMapClass::SetRenderTarget(ID3D11DeviceContext* deviceContext)
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	deviceContext->OMSetRenderTargets(1, &nullRTV, m_depthStencilView);

	deviceContext->RSSetViewports(1, &m_viewport);
}

void ShadowMapClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext)
{
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* ShadowMapClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

void ShadowMapClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void ShadowMapClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

int ShadowMapClass::GetShadowMapWidth()
{
	return m_shadowMapWidth;
}

int ShadowMapClass::GetShadowMapHeight()
{
	return m_shadowMapHeight;
}
