#ifndef _MOVINGOBJECTSCLASS_H_
#define _MOVINGOBJECTSCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace DirectX;

class MovingObjectsClass
{
public:
    enum MovementType { 
        RANDOM,         
        BOUNCING,       
        CIRCULAR,       
        PATROL,     
        SINE_WAVE      
    };

    enum ObjectShape {
        SPHERE,
        CUBE
    };

    struct MovingObject
    {
        XMFLOAT3 position;
        XMFLOAT3 velocity;
        XMFLOAT3 basePosition;   
        float scale;
        float rotation;
        MovementType movementType;
        ObjectShape shape;
        XMFLOAT4 color;
        
        float speed;
        float amplitude;         
        float phase;            
        float frequency;        
        bool movingUp;           
        float patrolLength;      
        float randomChangeTimer;  
    };

private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

public:
    MovingObjectsClass();
    ~MovingObjectsClass();

    bool Initialize(ID3D11Device* device);
    void Shutdown();

    void AddRandomMover(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float speed);
    void AddBouncingObject(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float amplitude, float speed);
    void AddCircularMover(float centerX, float centerY, float centerZ, float radius, float scale, ObjectShape shape, XMFLOAT4 color, float speed);
    void AddPatrolMover(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float patrolLength, float speed);
    void AddSineWaveMover(float x, float y, float z, float scale, ObjectShape shape, XMFLOAT4 color, float amplitude, float frequency, float speed);

    void Update(float frameTime, float terrainMinX, float terrainMaxX, float terrainMinZ, float terrainMaxZ);

    void Render(ID3D11DeviceContext* deviceContext, int objectIndex);
    int GetObjectCount();
    int GetIndexCount(int objectIndex);
    XMMATRIX GetObjectWorldMatrix(int objectIndex);
    XMFLOAT4 GetObjectColor(int objectIndex);

private:
    bool CreateSphereBuffers(ID3D11Device* device);
    bool CreateCubeBuffers(ID3D11Device* device);
    void ShutdownBuffers();

    void UpdateRandomMovement(MovingObject& obj, float frameTime, float minX, float maxX, float minZ, float maxZ);
    void UpdateBouncingMovement(MovingObject& obj, float frameTime);
    void UpdateCircularMovement(MovingObject& obj, float frameTime);
    void UpdatePatrolMovement(MovingObject& obj, float frameTime);
    void UpdateSineWaveMovement(MovingObject& obj, float frameTime);

    float RandomFloat(float min, float max);

private:
    std::vector<MovingObject> m_objects;
    
    ID3D11Buffer* m_sphereVertexBuffer;
    ID3D11Buffer* m_sphereIndexBuffer;
    int m_sphereIndexCount;

    ID3D11Buffer* m_cubeVertexBuffer;
    ID3D11Buffer* m_cubeIndexBuffer;
    int m_cubeIndexCount;

    bool m_initialized;
};

#endif
