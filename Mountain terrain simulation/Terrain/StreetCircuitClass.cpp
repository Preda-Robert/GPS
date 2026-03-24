#include "StreetCircuitClass.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

StreetCircuitClass::StreetCircuitClass()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_vertices = nullptr;
    m_indices = nullptr;
    m_vertexCount = 0;
    m_indexCount = 0;
}

StreetCircuitClass::~StreetCircuitClass()
{
}

bool StreetCircuitClass::Initialize(ID3D11Device* device, CircuitType type, float centerX, float centerZ, float width, float height, float roadWidth)
{
    m_circuitType = type;
    m_centerX = centerX;
    m_centerZ = centerZ;
    m_width = width;
    m_height = height;
    m_roadWidth = roadWidth;

    switch (type)
    {
    case CIRCULAR:
        BuildCircularCircuit(centerX, centerZ, width / 2.0f, roadWidth);
        break;
    case OVAL:
        BuildOvalCircuit(centerX, centerZ, width, height, roadWidth);
        break;
    case RECTANGULAR:
        BuildRectangularCircuit(centerX, centerZ, width, height, roadWidth);
        break;
    }

    return InitializeBuffers(device);
}

void StreetCircuitClass::BuildCircularCircuit(float centerX, float centerZ, float radius, float roadWidth)
{
    const int segments = 64;
    float innerRadius = radius - roadWidth / 2.0f;
    float outerRadius = radius + roadWidth / 2.0f;

    m_vertexCount = segments * 2;
    m_indexCount = segments * 6;

    m_vertices = new VertexType[m_vertexCount];
    m_indices = new unsigned long[m_indexCount];

    for (int i = 0; i < segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * PI;
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // Inner vertex
        int innerIdx = i * 2;
        m_vertices[innerIdx].position = XMFLOAT3(centerX + innerRadius * cosA, 0.5f, centerZ + innerRadius * sinA);
        m_vertices[innerIdx].texture = XMFLOAT2((float)i / segments * 4.0f, 0.0f);
        m_vertices[innerIdx].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

        // Outer vertex
        int outerIdx = i * 2 + 1;
        m_vertices[outerIdx].position = XMFLOAT3(centerX + outerRadius * cosA, 0.5f, centerZ + outerRadius * sinA);
        m_vertices[outerIdx].texture = XMFLOAT2((float)i / segments * 4.0f, 1.0f);
        m_vertices[outerIdx].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    }

    // Create indices for triangle strip
    int indexOffset = 0;
    for (int i = 0; i < segments; i++)
    {
        int current = i * 2;
        int next = ((i + 1) % segments) * 2;

        m_indices[indexOffset++] = current;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next;

        m_indices[indexOffset++] = next;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next + 1;
    }
}

void StreetCircuitClass::BuildOvalCircuit(float centerX, float centerZ, float width, float height, float roadWidth)
{
    const int segments = 64;
    float radiusX = width / 2.0f;
    float radiusZ = height / 2.0f;

    float innerRadiusX = radiusX - roadWidth / 2.0f;
    float outerRadiusX = radiusX + roadWidth / 2.0f;
    float innerRadiusZ = radiusZ - roadWidth / 2.0f;
    float outerRadiusZ = radiusZ + roadWidth / 2.0f;

    m_vertexCount = segments * 2;
    m_indexCount = segments * 6;

    m_vertices = new VertexType[m_vertexCount];
    m_indices = new unsigned long[m_indexCount];

    for (int i = 0; i < segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * PI;
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // Inner vertex
        int innerIdx = i * 2;
        m_vertices[innerIdx].position = XMFLOAT3(centerX + innerRadiusX * cosA, 0.5f, centerZ + innerRadiusZ * sinA);
        m_vertices[innerIdx].texture = XMFLOAT2((float)i / segments * 4.0f, 0.0f);
        m_vertices[innerIdx].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

        // Outer vertex
        int outerIdx = i * 2 + 1;
        m_vertices[outerIdx].position = XMFLOAT3(centerX + outerRadiusX * cosA, 0.5f, centerZ + outerRadiusZ * sinA);
        m_vertices[outerIdx].texture = XMFLOAT2((float)i / segments * 4.0f, 1.0f);
        m_vertices[outerIdx].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
    }

    int indexOffset = 0;
    for (int i = 0; i < segments; i++)
    {
        int current = i * 2;
        int next = ((i + 1) % segments) * 2;

        m_indices[indexOffset++] = current;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next;

        m_indices[indexOffset++] = next;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next + 1;
    }
}

void StreetCircuitClass::BuildRectangularCircuit(float centerX, float centerZ, float width, float height, float roadWidth)
{
    // Rectangular circuit with rounded corners
    const int cornerSegments = 8;
    const int totalSegments = 4 * cornerSegments + 4 * 2; // 4 corners + 4 straight sections (2 segments each)
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float cornerRadius = roadWidth;

    m_vertexCount = totalSegments * 2;
    m_indexCount = totalSegments * 6;

    m_vertices = new VertexType[m_vertexCount];
    m_indices = new unsigned long[m_indexCount];

    int vertexIndex = 0;
    float texCoord = 0.0f;

    auto addSegment = [&](float innerX, float innerZ, float outerX, float outerZ) {
        m_vertices[vertexIndex].position = XMFLOAT3(innerX, 0.5f, innerZ);
        m_vertices[vertexIndex].texture = XMFLOAT2(texCoord, 0.0f);
        m_vertices[vertexIndex].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        vertexIndex++;

        m_vertices[vertexIndex].position = XMFLOAT3(outerX, 0.5f, outerZ);
        m_vertices[vertexIndex].texture = XMFLOAT2(texCoord, 1.0f);
        m_vertices[vertexIndex].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        vertexIndex++;

        texCoord += 0.1f;
    };

    // Corner centers
    float cornerCenters[4][2] = {
        {centerX + halfWidth - cornerRadius, centerZ + halfHeight - cornerRadius},   // Top-right
        {centerX - halfWidth + cornerRadius, centerZ + halfHeight - cornerRadius},   // Top-left
        {centerX - halfWidth + cornerRadius, centerZ - halfHeight + cornerRadius},   // Bottom-left
        {centerX + halfWidth - cornerRadius, centerZ - halfHeight + cornerRadius}    // Bottom-right
    };

    // Build each side with corners
    for (int corner = 0; corner < 4; corner++)
    {
        // Straight section before corner
        float startAngle = corner * PI / 2.0f;
        
        // Corner arc
        for (int i = 0; i <= cornerSegments; i++)
        {
            float angle = startAngle + (float)i / cornerSegments * (PI / 2.0f);
            float cosA = cosf(angle);
            float sinA = sinf(angle);

            float innerR = cornerRadius - roadWidth / 2.0f;
            float outerR = cornerRadius + roadWidth / 2.0f;

            float innerX = cornerCenters[corner][0] + innerR * cosA;
            float innerZ = cornerCenters[corner][1] + innerR * sinA;
            float outerX = cornerCenters[corner][0] + outerR * cosA;
            float outerZ = cornerCenters[corner][1] + outerR * sinA;

            addSegment(innerX, innerZ, outerX, outerZ);
        }
    }

    // Adjust vertex and index counts
    m_vertexCount = vertexIndex;
    m_indexCount = (m_vertexCount / 2 - 1) * 6;

    // Generate indices
    int indexOffset = 0;
    for (int i = 0; i < m_vertexCount / 2 - 1; i++)
    {
        int current = i * 2;
        int next = (i + 1) * 2;

        m_indices[indexOffset++] = current;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next;

        m_indices[indexOffset++] = next;
        m_indices[indexOffset++] = current + 1;
        m_indices[indexOffset++] = next + 1;
    }

    // Close the loop
    int last = (m_vertexCount / 2 - 1) * 2;
    m_indices[indexOffset++] = last;
    m_indices[indexOffset++] = last + 1;
    m_indices[indexOffset++] = 0;

    m_indices[indexOffset++] = 0;
    m_indices[indexOffset++] = last + 1;
    m_indices[indexOffset++] = 1;

    m_indexCount = indexOffset;
}

bool StreetCircuitClass::InitializeBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    vertexData.pSysMem = m_vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
    {
        return false;
    }

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = m_indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
    {
        return false;
    }

    delete[] m_vertices;
    m_vertices = nullptr;

    delete[] m_indices;
    m_indices = nullptr;

    return true;
}

void StreetCircuitClass::Shutdown()
{
    ShutdownBuffers();
}

void StreetCircuitClass::ShutdownBuffers()
{
    if (m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    if (m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }

    if (m_vertices)
    {
        delete[] m_vertices;
        m_vertices = nullptr;
    }

    if (m_indices)
    {
        delete[] m_indices;
        m_indices = nullptr;
    }
}

void StreetCircuitClass::Render(ID3D11DeviceContext* deviceContext)
{
    RenderBuffers(deviceContext);
}

void StreetCircuitClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int StreetCircuitClass::GetIndexCount()
{
    return m_indexCount;
}

XMFLOAT3 StreetCircuitClass::GetPosition()
{
    return XMFLOAT3(m_centerX, 0.5f, m_centerZ);
}