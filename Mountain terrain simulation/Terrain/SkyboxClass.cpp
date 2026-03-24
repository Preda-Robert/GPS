#include "SkyboxClass.h"

SkyboxClass::SkyboxClass()
{
  m_vertexBuffer = 0;
  m_indexBuffer = 0;
  m_vertexCount = 0;
  m_indexCount = 0;
}

SkyboxClass::~SkyboxClass()
{
}

bool SkyboxClass::Initialize(ID3D11Device* device)
{
  return InitializeBuffers(device);
}

void SkyboxClass::Shutdown()
{
  ReleaseBuffers();
}

void SkyboxClass::Render(ID3D11DeviceContext* deviceContext)
{
  RenderBuffers(deviceContext);
}

int SkyboxClass::GetIndexCount()
{
  return m_indexCount;
}

bool SkyboxClass::InitializeBuffers(ID3D11Device* device)
{
  VertexType* vertices;
  unsigned long* indices;
  D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData, indexData;

  // Cube with 8 vertices, 36 indices (12 triangles)
  m_vertexCount = 8;
  m_indexCount = 36;

  vertices = new VertexType[m_vertexCount];
  indices = new unsigned long[m_indexCount];

  if (!vertices || !indices) {
    return false;
  }

  // Define cube vertices (size 1.0, will be scaled in shader)
  float size = 1.0f;

  // Front-bottom-left
  vertices[0].position = XMFLOAT3(-size, -size, -size);
  // Front-bottom-right
  vertices[1].position = XMFLOAT3(size, -size, -size);
  // Front-top-right
  vertices[2].position = XMFLOAT3(size, size, -size);
  // Front-top-left
  vertices[3].position = XMFLOAT3(-size, size, -size);
  // Back-bottom-left
  vertices[4].position = XMFLOAT3(-size, -size, size);
  // Back-bottom-right
  vertices[5].position = XMFLOAT3(size, -size, size);
  // Back-top-right
  vertices[6].position = XMFLOAT3(size, size, size);
  // Back-top-left
  vertices[7].position = XMFLOAT3(-size, size, size);

  // Define indices (winding order for inside of cube - we're inside looking out)
  int i = 0;

  // Front face
  indices[i++] = 0; indices[i++] = 2; indices[i++] = 1;
  indices[i++] = 0; indices[i++] = 3; indices[i++] = 2;

  // Back face
  indices[i++] = 5; indices[i++] = 7; indices[i++] = 4;
  indices[i++] = 5; indices[i++] = 6; indices[i++] = 7;

  // Left face
  indices[i++] = 4; indices[i++] = 3; indices[i++] = 0;
  indices[i++] = 4; indices[i++] = 7; indices[i++] = 3;

  // Right face
  indices[i++] = 1; indices[i++] = 6; indices[i++] = 5;
  indices[i++] = 1; indices[i++] = 2; indices[i++] = 6;

  // Top face
  indices[i++] = 3; indices[i++] = 6; indices[i++] = 2;
  indices[i++] = 3; indices[i++] = 7; indices[i++] = 6;

  // Bottom face
  indices[i++] = 4; indices[i++] = 1; indices[i++] = 5;
  indices[i++] = 4; indices[i++] = 0; indices[i++] = 1;

  // Create vertex buffer
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer))) {
    return false;
  }

  // Create index buffer
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  indexData.pSysMem = indices;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer))) {
    return false;
  }

  delete[] vertices;
  delete[] indices;

  return true;
}

void SkyboxClass::ReleaseBuffers()
{
  if (m_indexBuffer) {
    m_indexBuffer->Release();
    m_indexBuffer = NULL;
  }

  if (m_vertexBuffer) {
    m_vertexBuffer->Release();
    m_vertexBuffer = NULL;
  }
}

void SkyboxClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
  unsigned int stride = sizeof(VertexType);
  unsigned int offset = 0;

  deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
  deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
  deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}