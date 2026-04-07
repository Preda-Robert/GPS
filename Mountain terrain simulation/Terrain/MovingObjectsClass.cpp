#include "MovingObjectsClass.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

MovingObjectsClass::MovingObjectsClass()
{
    m_sphereVertexBuffer = nullptr;
    m_sphereIndexBuffer = nullptr;
    m_sphereIndexCount = 0;
    m_cubeVertexBuffer = nullptr;
    m_cubeIndexBuffer = nullptr;
    m_cubeIndexCount = 0;
    m_initialized = false;

    srand(static_cast<unsigned int>(time(nullptr)));
}

MovingObjectsClass::~MovingObjectsClass()
{
}

bool MovingObjectsClass::Initialize(ID3D11Device* device)
{
    if (!CreateSphereBuffers(device))
        return false;

    if (!CreateCubeBuffers(device))
        return false;

    m_initialized = true;
    return true;
}

void MovingObjectsClass::Shutdown()
{
    ShutdownBuffers();
    m_objects.clear();
}

float MovingObjectsClass::RandomFloat(float min, float max)
{
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void MovingObjectsClass::AddRandomMover(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float speed)
{
    MovingObject obj;
    obj.position = XMFLOAT3(x, y, z);
    obj.basePosition = XMFLOAT3(x, y, z);
    obj.scale = scale;
    obj.rotation = 0.0f;
    obj.movementType = RANDOM;
    obj.shape = shape;
    obj.color = color;
    obj.speed = speed;
    obj.amplitude = 0.0f;
    obj.phase = 0.0f;
    obj.frequency = 0.0f;
    obj.movingUp = true;
    obj.patrolLength = 0.0f;
    obj.randomChangeTimer = RandomFloat(1.0f, 3.0f);
    
    obj.velocity = XMFLOAT3(
        RandomFloat(-1.0f, 1.0f),
        0.0f,
        RandomFloat(-1.0f, 1.0f)
    );
    float len = sqrtf(obj.velocity.x * obj.velocity.x + obj.velocity.z * obj.velocity.z);
    if (len > 0.0f) {
        obj.velocity.x /= len;
        obj.velocity.z /= len;
    }

    m_objects.push_back(obj);
}

void MovingObjectsClass::AddBouncingObject(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float amplitude, float speed)
{
    MovingObject obj;
    obj.position = XMFLOAT3(x, y, z);
    obj.basePosition = XMFLOAT3(x, y, z);
    obj.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    obj.scale = scale;
    obj.rotation = 0.0f;
    obj.movementType = BOUNCING;
    obj.shape = shape;
    obj.color = color;
    obj.speed = speed;
    obj.amplitude = amplitude;
    obj.phase = 0.0f;
    obj.frequency = 0.0f;
    obj.movingUp = true;
    obj.patrolLength = 0.0f;
    obj.randomChangeTimer = 0.0f;

    m_objects.push_back(obj);
}

void MovingObjectsClass::AddCircularMover(float centerX, float centerY, float centerZ, float radius, float scale, ObjectShape shape, XMFLOAT4 color, float speed)
{
    MovingObject obj;
    obj.position = XMFLOAT3(centerX + radius, centerY, centerZ);
    obj.basePosition = XMFLOAT3(centerX, centerY, centerZ);
    obj.velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
    obj.scale = scale;
    obj.rotation = 0.0f;
    obj.movementType = CIRCULAR;
    obj.shape = shape;
    obj.color = color;
    obj.speed = speed;
    obj.amplitude = radius; 
    obj.phase = 0.0f;
    obj.frequency = speed;
    obj.movingUp = true;
    obj.patrolLength = 0.0f;
    obj.randomChangeTimer = 0.0f;

    m_objects.push_back(obj);
}

void MovingObjectsClass::AddPatrolMover(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float patrolLength, float speed)
{
    MovingObject obj;
    obj.position = XMFLOAT3(x, y, z);
    obj.basePosition = XMFLOAT3(x, y, z);
    obj.velocity = XMFLOAT3(1.0f, 0.0f, 0.0f);  
    obj.scale = scale;
    obj.rotation = 0.0f;
    obj.movementType = PATROL;
    obj.shape = shape;
    obj.color = color;
    obj.speed = speed;
    obj.amplitude = 0.0f;
    obj.phase = 0.0f;
    obj.frequency = 0.0f;
    obj.movingUp = true; 
    obj.patrolLength = patrolLength;
    obj.randomChangeTimer = 0.0f;

    m_objects.push_back(obj);
}


void MovingObjectsClass::Update(float frameTime, float terrainMinX, float terrainMaxX, float terrainMinZ, float terrainMaxZ)
{
    for (auto& obj : m_objects)
    {
        switch (obj.movementType)
        {
        case RANDOM:
            UpdateRandomMovement(obj, frameTime, terrainMinX, terrainMaxX, terrainMinZ, terrainMaxZ);
            break;
        case BOUNCING:
            UpdateBouncingMovement(obj, frameTime);
            break;
        case CIRCULAR:
            UpdateCircularMovement(obj, frameTime);
            break;
        case PATROL:
            UpdatePatrolMovement(obj, frameTime);
            break;
        }

        obj.rotation += frameTime * 0.05f;
        if (obj.rotation > 360.0f)
            obj.rotation -= 360.0f;
    }
}

void MovingObjectsClass::UpdateRandomMovement(MovingObject& obj, float frameTime, float minX, float maxX, float minZ, float maxZ)
{
    obj.randomChangeTimer -= frameTime;
    
    if (obj.randomChangeTimer <= 0.0f)
    {
        obj.velocity.x = RandomFloat(-1.0f, 1.0f);
        obj.velocity.z = RandomFloat(-1.0f, 1.0f);
        
        float len = sqrtf(obj.velocity.x * obj.velocity.x + obj.velocity.z * obj.velocity.z);
        if (len > 0.0f) {
            obj.velocity.x /= len;
            obj.velocity.z /= len;
        }
        
        obj.randomChangeTimer = RandomFloat(1.0f, 4.0f);
    }

    float moveAmount = obj.speed * frameTime;
    obj.position.x += obj.velocity.x * moveAmount;
    obj.position.z += obj.velocity.z * moveAmount;

    if (obj.position.x < minX || obj.position.x > maxX)
    {
        obj.velocity.x = -obj.velocity.x;
        obj.position.x = (obj.position.x < minX) ? minX : maxX;
    }
    if (obj.position.z < minZ || obj.position.z > maxZ)
    {
        obj.velocity.z = -obj.velocity.z;
        obj.position.z = (obj.position.z < minZ) ? minZ : maxZ;
    }
}

void MovingObjectsClass::UpdateBouncingMovement(MovingObject& obj, float frameTime)
{
    float moveAmount = obj.speed * frameTime;

    if (obj.movingUp)
    {
        obj.position.y += moveAmount;
        if (obj.position.y >= obj.basePosition.y + obj.amplitude)
        {
            obj.position.y = obj.basePosition.y + obj.amplitude;
            obj.movingUp = false;
        }
    }
    else
    {
        obj.position.y -= moveAmount;
        if (obj.position.y <= obj.basePosition.y)
        {
            obj.position.y = obj.basePosition.y;
            obj.movingUp = true;
        }
    }
}

void MovingObjectsClass::UpdateCircularMovement(MovingObject& obj, float frameTime)
{
    obj.phase += obj.frequency * frameTime;
    if (obj.phase > 2.0f * PI)
        obj.phase -= 2.0f * PI;

    float radius = obj.amplitude;
    obj.position.x = obj.basePosition.x + radius * cosf(obj.phase);
    obj.position.z = obj.basePosition.z + radius * sinf(obj.phase);
}

void MovingObjectsClass::UpdatePatrolMovement(MovingObject& obj, float frameTime)
{
    float moveAmount = obj.speed * frameTime;

    if (obj.movingUp)
    {
        obj.position.x += moveAmount;
        if (obj.position.x >= obj.basePosition.x + obj.patrolLength)
        {
            obj.position.x = obj.basePosition.x + obj.patrolLength;
            obj.movingUp = false;
        }
    }
    else
    {
        obj.position.x -= moveAmount;
        if (obj.position.x <= obj.basePosition.x - obj.patrolLength)
        {
            obj.position.x = obj.basePosition.x - obj.patrolLength;
            obj.movingUp = true;
        }
    }
}


void MovingObjectsClass::Render(ID3D11DeviceContext* deviceContext, int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return;

    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    if (m_objects[objectIndex].shape == SPHERE)
    {
        deviceContext->IASetVertexBuffers(0, 1, &m_sphereVertexBuffer, &stride, &offset);
        deviceContext->IASetIndexBuffer(m_sphereIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }
    else
    {
        deviceContext->IASetVertexBuffers(0, 1, &m_cubeVertexBuffer, &stride, &offset);
        deviceContext->IASetIndexBuffer(m_cubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    }
    
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int MovingObjectsClass::GetObjectCount()
{
    return (int)m_objects.size();
}

int MovingObjectsClass::GetIndexCount(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return 0;

    if (m_objects[objectIndex].shape == SPHERE)
        return m_sphereIndexCount;
    else
        return m_cubeIndexCount;
}

XMMATRIX MovingObjectsClass::GetObjectWorldMatrix(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return XMMatrixIdentity();

    const MovingObject& obj = m_objects[objectIndex];

    XMMATRIX scale = XMMatrixScaling(obj.scale, obj.scale, obj.scale);
    XMMATRIX rotation = XMMatrixRotationY(obj.rotation * (PI / 180.0f));
    XMMATRIX translation = XMMatrixTranslation(obj.position.x, obj.position.y, obj.position.z);

    return scale * rotation * translation;
}

XMFLOAT4 MovingObjectsClass::GetObjectColor(int objectIndex)
{
    if (objectIndex < 0 || objectIndex >= (int)m_objects.size())
        return XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    return m_objects[objectIndex].color;
}

bool MovingObjectsClass::CreateSphereBuffers(ID3D11Device* device)
{
    const int latSegments = 16;
    const int lonSegments = 16;
    
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    for (int lat = 0; lat <= latSegments; lat++)
    {
        float theta = lat * PI / latSegments;
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (int lon = 0; lon <= lonSegments; lon++)
        {
            float phi = lon * 2.0f * PI / lonSegments;
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            VertexType vertex;
            vertex.position = XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f);
            vertex.texture = XMFLOAT2((float)lon / lonSegments, (float)lat / latSegments);
            vertex.normal = XMFLOAT3(x, y, z);

            vertices.push_back(vertex);
        }
    }

    for (int lat = 0; lat < latSegments; lat++)
    {
        for (int lon = 0; lon < lonSegments; lon++)
        {
            int first = lat * (lonSegments + 1) + lon;
            int second = first + lonSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    m_sphereIndexCount = (int)indices.size();

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (UINT)vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_sphereVertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_sphereIndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_sphereIndexBuffer)))
        return false;

    return true;
}

bool MovingObjectsClass::CreateCubeBuffers(ID3D11Device* device)
{
    VertexType vertices[] = {
        // Front
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        // Back
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // Left
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        // Right
        {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // Top
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        // Bottom
        {{-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
    };

    unsigned long indices[] = {
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Left
        12, 13, 14, 12, 14, 15, // Right
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    m_cubeIndexCount = 36;

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * 24;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_cubeVertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_cubeIndexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_cubeIndexBuffer)))
        return false;

    return true;
}

void MovingObjectsClass::ShutdownBuffers()
{
    if (m_sphereIndexBuffer)
    {
        m_sphereIndexBuffer->Release();
        m_sphereIndexBuffer = nullptr;
    }
    if (m_sphereVertexBuffer)
    {
        m_sphereVertexBuffer->Release();
        m_sphereVertexBuffer = nullptr;
    }
    if (m_cubeIndexBuffer)
    {
        m_cubeIndexBuffer->Release();
        m_cubeIndexBuffer = nullptr;
    }
    if (m_cubeVertexBuffer)
    {
        m_cubeVertexBuffer->Release();
        m_cubeVertexBuffer = nullptr;
    }
}
