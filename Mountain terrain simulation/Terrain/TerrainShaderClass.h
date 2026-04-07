#ifndef _TERRAINSHADERCLASS_H_
#define _TERRAINSHADERCLASS_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;
using namespace std;

#define MAX_POINT_LIGHTS 8

class TerrainShaderClass
{
public:
	struct PointLightBufferType
	{
		XMFLOAT4 positions[MAX_POINT_LIGHTS];     // xyz = position, w = range
		XMFLOAT4 colors[MAX_POINT_LIGHTS];        // rgb = color, a = intensity
		XMFLOAT4 attenuation[MAX_POINT_LIGHTS];   // x = constant, y = linear, z = quadratic, w = enabled
		int numLights;
		XMFLOAT3 padding;
	};

private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};
	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float shadowMapBias;
	};
public:
	TerrainShaderClass();
	~TerrainShaderClass();
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX,
		XMMATRIX, XMMATRIX,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4,
		PointLightBufferType*);
private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX,
		XMMATRIX, XMMATRIX,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*,
		XMFLOAT3, XMFLOAT4,
		PointLightBufferType*);
	void RenderShader(ID3D11DeviceContext*, int);
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11SamplerState* m_shadowSampleState;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_pointLightBuffer;
};

#endif