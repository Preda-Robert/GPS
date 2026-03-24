#include "SkyboxShaderClass.h"

SkyboxShaderClass::SkyboxShaderClass()
{
  m_vertexShader = NULL;
  m_pixelShader = NULL;
  m_layout = NULL;
  m_matrixBuffer = NULL;
  m_sampleState = NULL;
}

SkyboxShaderClass::~SkyboxShaderClass()
{
}

bool SkyboxShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
  return InitializeShader(device, hwnd, L"shaders/vs/Skybox.vs", L"shaders/ps/Skybox.ps");
}

void SkyboxShaderClass::Shutdown()
{
  ShutdownShader();
}

bool SkyboxShaderClass::Render(
  ID3D11DeviceContext* deviceContext,
  int indexCount,
  XMMATRIX worldMatrix,
  XMMATRIX viewMatrix,
  XMMATRIX projectionMatrix,
  ID3D11ShaderResourceView* cubemapTexture)
{
  if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, cubemapTexture)) {
    return false;
  }

  RenderShader(deviceContext, indexCount);
  return true;
}

bool SkyboxShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
  ID3D10Blob* errorMessage = 0;
  ID3D10Blob* vertexShaderBuffer = 0;
  ID3D10Blob* pixelShaderBuffer = 0;
  D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
  unsigned int numElements;
  D3D11_BUFFER_DESC matrixBufferDesc;
  D3D11_SAMPLER_DESC samplerDesc;

  // Compile vertex shader
  if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "SkyboxVertexShader", "vs_5_0",
    D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage))) {
    if (errorMessage) {
      OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
    }
    else {
      MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
    }
    return false;
  }

  // Compile pixel shader
  if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "SkyboxPixelShader", "ps_5_0",
    D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage))) {
    if (errorMessage) {
      OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
    }
    else {
      MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
    }
    return false;
  }

  // Create shaders
  if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
    vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)) ||
    FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
      pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader))) {
    return false;
  }

  // Input layout
  polygonLayout[0].SemanticName = "POSITION";
  polygonLayout[0].SemanticIndex = 0;
  polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  polygonLayout[0].InputSlot = 0;
  polygonLayout[0].AlignedByteOffset = 0;
  polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  polygonLayout[0].InstanceDataStepRate = 0;

  numElements = 1;

  if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
    vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout))) {
    return false;
  }

  vertexShaderBuffer->Release();
  pixelShaderBuffer->Release();

  // Matrix constant buffer
  matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
  matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrixBufferDesc.MiscFlags = 0;
  matrixBufferDesc.StructureByteStride = 0;

  if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer))) {
    return false;
  }

  // Sampler state for cubemap
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState))) {
    return false;
  }

  return true;
}

void SkyboxShaderClass::ShutdownShader()
{
  if (m_sampleState) {
    m_sampleState->Release();
    m_sampleState = NULL;
  }

  if (m_matrixBuffer) {
    m_matrixBuffer->Release();
    m_matrixBuffer = NULL;
  }

  if (m_layout) {
    m_layout->Release();
    m_layout = NULL;
  }

  if (m_pixelShader) {
    m_pixelShader->Release();
    m_pixelShader = NULL;
  }

  if (m_vertexShader) {
    m_vertexShader->Release();
    m_vertexShader = NULL;
  }
}

void SkyboxShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
  char* compileErrors = (char*)errorMessage->GetBufferPointer();
  unsigned __int64 bufferSize = errorMessage->GetBufferSize();
  ofstream fout;

  fout.open("shader-error.txt");
  for (unsigned __int64 i = 0; i < bufferSize; ++i) {
    fout << compileErrors[i];
  }
  fout.close();

  errorMessage->Release();
  MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

bool SkyboxShaderClass::SetShaderParameters(
  ID3D11DeviceContext* deviceContext,
  XMMATRIX worldMatrix,
  XMMATRIX viewMatrix,
  XMMATRIX projectionMatrix,
  ID3D11ShaderResourceView* cubemapTexture)
{
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  MatrixBufferType* dataPtr;

  worldMatrix = XMMatrixTranspose(worldMatrix);
  viewMatrix = XMMatrixTranspose(viewMatrix);
  projectionMatrix = XMMatrixTranspose(projectionMatrix);

  if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
    return false;
  }

  dataPtr = (MatrixBufferType*)mappedResource.pData;
  dataPtr->world = worldMatrix;
  dataPtr->view = viewMatrix;
  dataPtr->projection = projectionMatrix;

  deviceContext->Unmap(m_matrixBuffer, 0);
  deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
  deviceContext->PSSetShaderResources(0, 1, &cubemapTexture);
  deviceContext->PSSetSamplers(0, 1, &m_sampleState);

  return true;
}

void SkyboxShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
  deviceContext->IASetInputLayout(m_layout);
  deviceContext->VSSetShader(m_vertexShader, NULL, 0);
  deviceContext->PSSetShader(m_pixelShader, NULL, 0);
  deviceContext->DrawIndexed(indexCount, 0, 0);
}