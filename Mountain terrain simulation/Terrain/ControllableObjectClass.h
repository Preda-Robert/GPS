#ifndef _CONTROLLABLEOBJECTCLASS_H_
#define _CONTROLLABLEOBJECTCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class StaticObjectsClass;

class ControllableObjectClass
{
public:
    enum ObjectType { CAR };

    struct BoundingBox
    {
        XMFLOAT3 min;
        XMFLOAT3 max;
    };

private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

public:
    ControllableObjectClass();
    ~ControllableObjectClass();

    bool Initialize(ID3D11Device* device, ObjectType type);
    void Shutdown();

    void SetFrameTime(float frameTime);
    void MoveForward(bool keyDown);
    void MoveBackward(bool keyDown);
    void TurnLeft(bool keyDown);
    void TurnRight(bool keyDown);

    void Update(StaticObjectsClass* staticObjects, float terrainHeight);

    void Render(ID3D11DeviceContext* deviceContext);
    int GetIndexCount();
    XMMATRIX GetWorldMatrix();

    void GetPosition(float& x, float& y, float& z);
    void SetPosition(float x, float y, float z);
    void GetRotation(float& rotY);
    void SetRotation(float rotY);

    BoundingBox GetBoundingBox();
    bool CheckCollision(const BoundingBox& other);

    ObjectType GetObjectType();

private:
    bool CreateCarBuffers(ID3D11Device* device);
    void ShutdownBuffers();

    bool CheckCollisionWithStaticObjects(StaticObjectsClass* staticObjects, float newX, float newZ);

private:
    ObjectType m_objectType;
    
    XMFLOAT3 m_position;
    float m_rotationY;
    float m_forwardSpeed;
    float m_backwardSpeed;
    float m_turnSpeed;
    float m_frameTime;

    float m_currentForwardSpeed;
    float m_currentTurnSpeed;

    float m_width;
    float m_height;
    float m_depth;

    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    int m_indexCount;
};

#endif
