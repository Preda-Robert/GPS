#include "StaticObjectsClass.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

StaticObjectsClass::StaticObjectsClass()
{
    m_initialized = false;
    m_buildingBuffers.vertexBuffer = nullptr;
    m_buildingBuffers.indexBuffer = nullptr;
    m_treeBuffers.vertexBuffer = nullptr;
    m_treeBuffers.indexBuffer = nullptr;
    m_lamppostBuffers.vertexBuffer = nullptr;
    m_lamppostBuffers.indexBuffer = nullptr;
    m_barrierBuffers.vertexBuffer = nullptr;
    m_barrierBuffers.indexBuffer = nullptr;
    m_coneBuffers.vertexBuffer = nullptr;
    m_coneBuffers.indexBuffer = nullptr;
}

StaticObjectsClass::~StaticObjectsClass()
{
}

bool StaticObjectsClass::Initialize(ID3D11Device* device)
{
    if (!CreateBuildingBuffers(device, m_buildingBuffers) ||
        !CreateTreeBuffers(device, m_treeBuffers) ||
        !CreateLamppostBuffers(device, m_lamppostBuffers) ||
        !CreateBarrierBuffers(device, m_barrierBuffers) ||
        !CreateConeBuffers(device, m_coneBuffers))
    {
        return false;
    }

    m_initialized = true;
    return true;
}

void StaticObjectsClass::AddObject(ObjectType type, float x, float y, float z, float scaleX, float scaleY, float scaleZ, float rotation)
{
    StaticObject obj;
    obj.type = type;
    obj.position = XMFLOAT3(x, y, z);
    obj.scale = XMFLOAT3(scaleX, scaleY, scaleZ);
    obj.rotation = rotation;
    m_objects.push_back(obj);
}

bool StaticObjectsClass::CreateBuildingBuffers(ID3D11Device* device, ObjectBuffers& buffers)
{
    // Simple box building
    VertexType vertices[] = {
        // Front face
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f, 1.0f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, 1.0f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, 0.0f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        // Back face
        {{ 0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, 1.0f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 1.0f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // Left face
        {{-0.5f, 0.0f,  0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 1.0f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 1.0f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, 0.0f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        // Right face
        {{ 0.5f, 0.0f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, 1.0f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, 1.0f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, 0.0f,  0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // Top face
        {{-0.5f, 1.0f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 1.0f,  0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f, 1.0f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f, 1.0f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    };

    unsigned long indices[] = {
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Left
        12, 13, 14, 12, 14, 15, // Right
        16, 17, 18, 16, 18, 19  // Top
    };

    int vertexCount = 20;
    buffers.indexCount = 30;
    buffers.type = BUILDING;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffers.vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * buffers.indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &buffers.indexBuffer)))
        return false;

    return true;
}

bool StaticObjectsClass::CreateTreeBuffers(ID3D11Device* device, ObjectBuffers& buffers)
{
    // Cone-shaped tree with trunk
    const int segments = 12;
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    // Trunk (cylinder)
    float trunkRadius = 0.1f;
    float trunkHeight = 0.4f;
    
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;
        float x = trunkRadius * cosf(angle);
        float z = trunkRadius * sinf(angle);
        
        // Bottom vertex
        vertices.push_back({{x, 0.0f, z}, {(float)i / segments, 1.0f}, {cosf(angle), 0.0f, sinf(angle)}});
        // Top vertex
        vertices.push_back({{x, trunkHeight, z}, {(float)i / segments, 0.0f}, {cosf(angle), 0.0f, sinf(angle)}});
    }

    // Trunk indices
    for (int i = 0; i < segments; i++)
    {
        int base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }

    // Foliage (cone)
    int coneBase = (int)vertices.size();
    float coneRadius = 0.5f;
    float coneHeight = 1.0f;
    float coneYBase = trunkHeight;

    // Cone apex
    vertices.push_back({{0.0f, coneYBase + coneHeight, 0.0f}, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}});

    // Cone base vertices
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;
        float x = coneRadius * cosf(angle);
        float z = coneRadius * sinf(angle);
        
        XMFLOAT3 normal;
        float ny = coneRadius / sqrtf(coneRadius * coneRadius + coneHeight * coneHeight);
        float nxz = coneHeight / sqrtf(coneRadius * coneRadius + coneHeight * coneHeight);
        normal = {nxz * cosf(angle), ny, nxz * sinf(angle)};
        
        vertices.push_back({{x, coneYBase, z}, {(float)i / segments, 1.0f}, normal});
    }

    // Cone indices
    for (int i = 0; i < segments; i++)
    {
        indices.push_back(coneBase);  // apex
        indices.push_back(coneBase + 1 + i);
        indices.push_back(coneBase + 1 + i + 1);
    }

    buffers.indexCount = (int)indices.size();
    buffers.type = TREE;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (UINT)vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffers.vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * buffers.indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &buffers.indexBuffer)))
        return false;

    return true;
}

bool StaticObjectsClass::CreateLamppostBuffers(ID3D11Device* device, ObjectBuffers& buffers)
{
    // Simple lamppost: thin pole + box light
    VertexType vertices[] = {
        // Pole (thin box)
        {{-0.05f, 0.0f, -0.05f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.05f, 1.0f, -0.05f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{ 0.05f, 1.0f, -0.05f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.05f, 0.0f, -0.05f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.05f, 0.0f,  0.05f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.05f, 1.0f,  0.05f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{ 0.05f, 1.0f,  0.05f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.05f, 0.0f,  0.05f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // Light box at top
        {{-0.15f, 0.95f, -0.15f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.15f, 1.05f, -0.15f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.15f, 1.05f, -0.15f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.15f, 0.95f, -0.15f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.15f, 0.95f,  0.15f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.15f, 1.05f,  0.15f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.15f, 1.05f,  0.15f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.15f, 0.95f,  0.15f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    };

    unsigned long indices[] = {
        // Pole
        0, 1, 5, 0, 5, 4,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 2, 6, 1, 6, 5,
        // Light box
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        8, 9, 13, 8, 13, 12,
        11, 10, 14, 11, 14, 15,
        9, 13, 14, 9, 14, 10,
        8, 12, 15, 8, 15, 11,
    };

    int vertexCount = 16;
    buffers.indexCount = 60;
    buffers.type = LAMPPOST;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffers.vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * buffers.indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &buffers.indexBuffer)))
        return false;

    return true;
}

bool StaticObjectsClass::CreateBarrierBuffers(ID3D11Device* device, ObjectBuffers& buffers)
{
    // Road barrier - simple elongated box
    VertexType vertices[] = {
        // Front
        {{-1.0f, 0.0f, -0.1f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-1.0f, 0.3f, -0.1f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 1.0f, 0.3f, -0.1f}, {4.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 1.0f, 0.0f, -0.1f}, {4.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        // Back
        {{ 1.0f, 0.0f,  0.1f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 1.0f, 0.3f,  0.1f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, 0.3f,  0.1f}, {4.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, 0.0f,  0.1f}, {4.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // Top
        {{-1.0f, 0.3f, -0.1f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, 0.3f,  0.1f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 1.0f, 0.3f,  0.1f}, {4.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 1.0f, 0.3f, -0.1f}, {4.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    };

    unsigned long indices[] = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11
    };

    buffers.indexCount = 18;
    buffers.type = BARRIER;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * 12;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffers.vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * buffers.indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &buffers.indexBuffer)))
        return false;

    return true;
}

bool StaticObjectsClass::CreateConeBuffers(ID3D11Device* device, ObjectBuffers& buffers)
{
    // Traffic cone
    const int segments = 8;
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    float baseRadius = 0.15f;
    float topRadius = 0.03f;
    float height = 0.3f;

    // Base circle center
    vertices.push_back({{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}});

    // Cone surface
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / segments * 2.0f * PI;
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        // Bottom vertex
        vertices.push_back({{baseRadius * cosA, 0.0f, baseRadius * sinA}, 
                           {(float)i / segments, 1.0f}, 
                           {cosA, 0.3f, sinA}});
        // Top vertex
        vertices.push_back({{topRadius * cosA, height, topRadius * sinA}, 
                           {(float)i / segments, 0.0f}, 
                           {cosA, 0.3f, sinA}});
    }

    // Base indices
    for (int i = 0; i < segments; i++)
    {
        indices.push_back(0);
        indices.push_back(1 + i * 2);
        indices.push_back(1 + (i + 1) * 2);
    }

    // Side indices
    for (int i = 0; i < segments; i++)
    {
        int base = 1 + i * 2;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);

        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
    }

    buffers.indexCount = (int)indices.size();
    buffers.type = CONE;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (UINT)vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &buffers.vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * buffers.indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &buffers.indexBuffer)))
        return false;

    return true;
}

void StaticObjectsClass::Shutdown()
{
    ShutdownBuffers();
    m_objects.clear();
}

void StaticObjectsClass::ShutdownBuffers()
{
    auto releaseBuffers = [](ObjectBuffers& b) {
        if (b.vertexBuffer) { b.vertexBuffer->Release(); b.vertexBuffer = nullptr; }
        if (b.indexBuffer) { b.indexBuffer->Release(); b.indexBuffer = nullptr; }
    };

    releaseBuffers(m_buildingBuffers);
    releaseBuffers(m_treeBuffers);
    releaseBuffers(m_lamppostBuffers);
    releaseBuffers(m_barrierBuffers);
    releaseBuffers(m_coneBuffers);
}

void StaticObjectsClass::Render(ID3D11DeviceContext* deviceContext, int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return;

    ObjectBuffers* buffers = nullptr;
    switch (m_objects[objectIndex].type)
    {
    case BUILDING:  buffers = &m_buildingBuffers; break;
    case TREE:      buffers = &m_treeBuffers; break;
    case LAMPPOST:  buffers = &m_lamppostBuffers; break;
    case BARRIER:   buffers = &m_barrierBuffers; break;
    case CONE:      buffers = &m_coneBuffers; break;
    }

    if (!buffers) return;

    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &buffers->vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(buffers->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int StaticObjectsClass::GetObjectCount()
{
    return (int)m_objects.size();
}

int StaticObjectsClass::GetIndexCount(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return 0;

    switch (m_objects[objectIndex].type)
    {
    case BUILDING:  return m_buildingBuffers.indexCount;
    case TREE:      return m_treeBuffers.indexCount;
    case LAMPPOST:  return m_lamppostBuffers.indexCount;
    case BARRIER:   return m_barrierBuffers.indexCount;
    case CONE:      return m_coneBuffers.indexCount;
    }
    return 0;
}

StaticObjectsClass::ObjectType StaticObjectsClass::GetObjectType(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return BUILDING;
    return m_objects[objectIndex].type;
}

XMMATRIX StaticObjectsClass::GetObjectWorldMatrix(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return XMMatrixIdentity();

    const StaticObject& obj = m_objects[objectIndex];
    
    XMMATRIX scale = XMMatrixScaling(obj.scale.x, obj.scale.y, obj.scale.z);
    XMMATRIX rotation = XMMatrixRotationY(obj.rotation);
    XMMATRIX translation = XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

    return scale * rotation * translation;
}