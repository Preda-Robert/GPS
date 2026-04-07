#include "ControllableObjectClass.h"
#include "StaticObjectsClass.h"
#include <cmath>

const float PI = 3.14159265358979323846f;

ControllableObjectClass::ControllableObjectClass()
{
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_indexCount = 0;

    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotationY = 0.0f;
    
    m_forwardSpeed = 0.0f;
    m_backwardSpeed = 0.0f;
    m_turnSpeed = 0.0f;
    m_frameTime = 0.0f;

    m_currentForwardSpeed = 0.0f;
    m_currentTurnSpeed = 0.0f;

    m_width = 1.0f;
    m_height = 1.0f;
    m_depth = 1.0f;

    m_objectType = PEDESTRIAN;
}

ControllableObjectClass::~ControllableObjectClass()
{
}

bool ControllableObjectClass::Initialize(ID3D11Device* device, ObjectType type)
{
    m_objectType = type;
    m_forwardSpeed = 35.0f;
    m_backwardSpeed = 20.0f;
    m_turnSpeed = 180.0f;
    m_width = 2.0f;
    m_height = 1.5f;
    m_depth = 4.0f;

    return CreateCarBuffers(device);

    return false;
}

void ControllableObjectClass::Shutdown()
{
    ShutdownBuffers();
}

void ControllableObjectClass::SetFrameTime(float frameTime)
{
    m_frameTime = frameTime;
}

void ControllableObjectClass::MoveForward(bool keyDown)
{
  float acceleration = 20.0f; 
  if (keyDown)
  {
    m_currentForwardSpeed += acceleration * m_frameTime;

    if (m_currentForwardSpeed > m_forwardSpeed)
      m_currentForwardSpeed = m_forwardSpeed;
  }
  else
  {
    m_currentForwardSpeed -= acceleration * m_frameTime;

    if (m_currentForwardSpeed < 0.0f)
      m_currentForwardSpeed = 0.0f;
  }
}

void ControllableObjectClass::MoveBackward(bool keyDown)
{
  if (keyDown)
  {
    m_currentForwardSpeed -= m_backwardSpeed * m_frameTime;

    if (m_currentForwardSpeed < -m_backwardSpeed)
      m_currentForwardSpeed = -m_backwardSpeed;
  }
}

void ControllableObjectClass::TurnLeft(bool keyDown)
{
  if (keyDown)
    m_currentTurnSpeed = -m_turnSpeed * m_frameTime;
}

void ControllableObjectClass::TurnRight(bool keyDown)
{
  if (keyDown)
    m_currentTurnSpeed = m_turnSpeed * m_frameTime;
}

bool ControllableObjectClass::CheckCollisionWithStaticObjects(StaticObjectsClass* staticObjects, float newX, float newZ)
{
    if (!staticObjects) return false;

    BoundingBox newBounds;
    newBounds.min = XMFLOAT3(newX - m_width / 2.0f, m_position.y, newZ - m_depth / 2.0f);
    newBounds.max = XMFLOAT3(newX + m_width / 2.0f, m_position.y + m_height, newZ + m_depth / 2.0f);

    for (int i = 0; i < staticObjects->GetObjectCount(); i++)
    {
        StaticObjectsClass::ObjectType objType = staticObjects->GetObjectType(i);
        
        if (objType == StaticObjectsClass::BUILDING || objType == StaticObjectsClass::BARRIER)
        {
          StaticObjectsClass::BoundingBox staticBox = staticObjects->GetObjectBoundingBox(i);

          BoundingBox objBounds;
          objBounds.min = staticBox.min;
          objBounds.max = staticBox.max;
            
            if (newBounds.min.x <= objBounds.max.x && newBounds.max.x >= objBounds.min.x &&
                newBounds.min.y <= objBounds.max.y && newBounds.max.y >= objBounds.min.y &&
                newBounds.min.z <= objBounds.max.z && newBounds.max.z >= objBounds.min.z)
            {
                return true;
            }
        }
    }

    return false;
}

void ControllableObjectClass::Update(StaticObjectsClass* staticObjects, float terrainHeight)
{
  m_rotationY += m_currentTurnSpeed;
  m_currentTurnSpeed = 0.0f;
    if (m_rotationY < 0.0f)
    {
        m_rotationY += 360.0f;
    }
    if (m_rotationY > 360.0f)
    {
        m_rotationY -= 360.0f;
    }

    float radians = m_rotationY * (PI / 180.0f);
    float moveStep = m_currentForwardSpeed * m_frameTime;

    float newX = m_position.x + sinf(radians) * moveStep;
    float newZ = m_position.z + cosf(radians) * moveStep;

    if (!CheckCollisionWithStaticObjects(staticObjects, newX, newZ))
    {
        m_position.x = newX;
        m_position.z = newZ;
    }
    else
    {
        m_currentForwardSpeed = 0.0f;
    }

    m_position.y = terrainHeight;
}

void ControllableObjectClass::Render(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride = sizeof(VertexType);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

int ControllableObjectClass::GetIndexCount()
{
    return m_indexCount;
}

XMMATRIX ControllableObjectClass::GetWorldMatrix()
{
    XMMATRIX scale = XMMatrixScaling(m_width, m_height, m_depth);
    XMMATRIX rotation = XMMatrixRotationY(m_rotationY * (PI / 180.0f));
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

    return scale * rotation * translation;
}

void ControllableObjectClass::GetPosition(float& x, float& y, float& z)
{
    x = m_position.x;
    y = m_position.y;
    z = m_position.z;
}

void ControllableObjectClass::SetPosition(float x, float y, float z)
{
    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
}

void ControllableObjectClass::GetRotation(float& rotY)
{
    rotY = m_rotationY;
}

void ControllableObjectClass::SetRotation(float rotY)
{
    m_rotationY = rotY;
}

ControllableObjectClass::BoundingBox ControllableObjectClass::GetBoundingBox()
{
    BoundingBox box;
    box.min = XMFLOAT3(m_position.x - m_width / 2.0f, m_position.y, m_position.z - m_depth / 2.0f);
    box.max = XMFLOAT3(m_position.x + m_width / 2.0f, m_position.y + m_height, m_position.z + m_depth / 2.0f);
    return box;
}

bool ControllableObjectClass::CheckCollision(const BoundingBox& other)
{
    BoundingBox myBox = GetBoundingBox();

    return (myBox.min.x <= other.max.x && myBox.max.x >= other.min.x &&
            myBox.min.y <= other.max.y && myBox.max.y >= other.min.y &&
            myBox.min.z <= other.max.z && myBox.max.z >= other.min.z);
}

ControllableObjectClass::ObjectType ControllableObjectClass::GetObjectType()
{
    return m_objectType;
}

bool ControllableObjectClass::CreateCarBuffers(ID3D11Device* device)
{
    std::vector<VertexType> vertices;
    std::vector<unsigned long> indices;

    float bodyWidth = 0.5f;
    float bodyHeight = 0.25f;
    float bodyDepth = 1.0f;

    VertexType bodyVerts[] = {
        // Front
        {{-bodyWidth, 0.0f, bodyDepth}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-bodyWidth, bodyHeight, bodyDepth}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ bodyWidth, bodyHeight, bodyDepth}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ bodyWidth, 0.0f, bodyDepth}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // Back
        {{ bodyWidth, 0.0f, -bodyDepth}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{ bodyWidth, bodyHeight, -bodyDepth}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-bodyWidth, bodyHeight, -bodyDepth}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-bodyWidth, 0.0f, -bodyDepth}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        // Left
        {{-bodyWidth, 0.0f, -bodyDepth}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-bodyWidth, bodyHeight, -bodyDepth}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-bodyWidth, bodyHeight, bodyDepth}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-bodyWidth, 0.0f, bodyDepth}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        // Right
        {{ bodyWidth, 0.0f, bodyDepth}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ bodyWidth, bodyHeight, bodyDepth}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ bodyWidth, bodyHeight, -bodyDepth}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ bodyWidth, 0.0f, -bodyDepth}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // Top
        {{-bodyWidth, bodyHeight, bodyDepth}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-bodyWidth, bodyHeight, -bodyDepth}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ bodyWidth, bodyHeight, -bodyDepth}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ bodyWidth, bodyHeight, bodyDepth}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        // Bottom
        {{-bodyWidth, 0.0f, -bodyDepth}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
        {{-bodyWidth, 0.0f, bodyDepth}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ bodyWidth, 0.0f, bodyDepth}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
        {{ bodyWidth, 0.0f, -bodyDepth}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
    };

    for (int i = 0; i < 24; i++)
        vertices.push_back(bodyVerts[i]);

    unsigned long bodyIndices[] = {
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Left
        12, 13, 14, 12, 14, 15, // Right
        16, 17, 18, 16, 18, 19, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    for (int i = 0; i < 36; i++)
        indices.push_back(bodyIndices[i]);

    int cabinBase = (int)vertices.size();
    float cabinWidth = 0.45f;
    float cabinHeight = 0.2f;
    float cabinFront = 0.3f;
    float cabinBack = -0.5f;

    VertexType cabinVerts[] = {
        // Front
        {{-cabinWidth, bodyHeight, cabinFront}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-cabinWidth, bodyHeight + cabinHeight, cabinFront}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinFront}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ cabinWidth, bodyHeight, cabinFront}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
        // Back
        {{ cabinWidth, bodyHeight, cabinBack}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinBack}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-cabinWidth, bodyHeight + cabinHeight, cabinBack}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
        {{-cabinWidth, bodyHeight, cabinBack}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
        // Left
        {{-cabinWidth, bodyHeight, cabinBack}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-cabinWidth, bodyHeight + cabinHeight, cabinBack}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-cabinWidth, bodyHeight + cabinHeight, cabinFront}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
        {{-cabinWidth, bodyHeight, cabinFront}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}},
        // Right
        {{ cabinWidth, bodyHeight, cabinFront}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinFront}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinBack}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ cabinWidth, bodyHeight, cabinBack}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
        // Top
        {{-cabinWidth, bodyHeight + cabinHeight, cabinFront}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{-cabinWidth, bodyHeight + cabinHeight, cabinBack}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinBack}, {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        {{ cabinWidth, bodyHeight + cabinHeight, cabinFront}, {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    };

    for (int i = 0; i < 20; i++)
        vertices.push_back(cabinVerts[i]);

    unsigned long cabinIndices[] = {
        0, 1, 2, 0, 2, 3,       // Front
        4, 5, 6, 4, 6, 7,       // Back
        8, 9, 10, 8, 10, 11,    // Left
        12, 13, 14, 12, 14, 15, // Right
        16, 17, 18, 16, 18, 19  // Top
    };

    for (int i = 0; i < 30; i++)
        indices.push_back(cabinBase + cabinIndices[i]);

    m_indexCount = (int)indices.size();

    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (UINT)vertices.size();
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
        return false;

    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
        return false;

    return true;
}

void ControllableObjectClass::ShutdownBuffers()
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
}
