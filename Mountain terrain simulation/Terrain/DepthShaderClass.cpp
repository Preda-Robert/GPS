#include "DepthShaderClass.h"

DepthShaderClass::DepthShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
}

DepthShaderClass::~DepthShaderClass()
{
}

bool DepthShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"shaders/vs/Depth.vs", L"shaders/ps/Depth.ps");
}

void DepthShaderClass::Shutdown()
{
	ShutdownShader();
}

bool DepthShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix)) {
		return false;
	}

	RenderShader(deviceContext, indexCount);
	return true;
}

bool DepthShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[7];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	result = D3DCompileFromFile(vsFilename, NULL, NULL, "DepthVertexShader", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		return false;
	}

	result = D3DCompileFromFile(psFilename, NULL, NULL, "DepthPixelShader", "ps_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result)) {
		if (errorMessage) {
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		return false;
	}

	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) {
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) {
		return false;
	}

	// vertex input layout based on terrain vertex format
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "COLOR";
	polygonLayout[5].SemanticIndex = 0;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	polygonLayout[6].SemanticName = "TEXCOORD";
	polygonLayout[6].SemanticIndex = 1;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[6].InputSlot = 0;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[6].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) {
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void DepthShaderClass::ShutdownShader()
{
	if (m_matrixBuffer) {
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout) {
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader) {
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader) {
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void DepthShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());
	bufferSize = errorMessage->GetBufferSize();

	fout.open("shader-error.txt");
	for (unsigned long long i = 0; i < bufferSize; i++) {
		fout << compileErrors[i];
	}
	fout.close();

	errorMessage->Release();
	errorMessage = nullptr;
}

bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

void DepthShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
