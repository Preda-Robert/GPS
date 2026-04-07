#ifndef _SHADOWMAPCLASS_H_
#define _SHADOWMAPCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>

using namespace DirectX;

class ShadowMapClass
{
public:
	ShadowMapClass();
	~ShadowMapClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*);
	void ClearRenderTarget(ID3D11DeviceContext*);
	ID3D11ShaderResourceView* GetShaderResourceView();
	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	int GetShadowMapWidth();
	int GetShadowMapHeight();

private:
	int m_shadowMapWidth, m_shadowMapHeight;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif
