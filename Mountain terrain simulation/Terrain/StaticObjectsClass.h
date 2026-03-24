#ifndef _STATICOBJECTSCLASS_H_
#define _STATICOBJECTSCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class StaticObjectsClass
{
public:
    enum ObjectType { BUILDING, TREE, LAMPPOST, BARRIER, CONE };

    struct StaticObject
    {
        ObjectType type;
        XMFLOAT3 position;
        XMFLOAT3 scale;
        float rotation;
    };

private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

    struct ObjectBuffers
    {
        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;
        int indexCount;
        ObjectType type;
    };

public:
    StaticObjectsClass();
    ~StaticObjectsClass();

    bool Initialize(ID3D11Device* device);
    void Shutdown();

    void AddObject(ObjectType type, float x, float y, float z, float scaleX, float scaleY, float scaleZ, float rotation);
    void Render(ID3D11DeviceContext* deviceContext, int objectIndex);
    
    int GetObjectCount();
    int GetIndexCount(int objectIndex);
    ObjectType GetObjectType(int objectIndex);
    XMMATRIX GetObjectWorldMatrix(int objectIndex);

private:
    bool CreateBuildingBuffers(ID3D11Device* device, ObjectBuffers& buffers);
    bool CreateTreeBuffers(ID3D11Device* device, ObjectBuffers& buffers);
    bool CreateLamppostBuffers(ID3D11Device* device, ObjectBuffers& buffers);
    bool CreateBarrierBuffers(ID3D11Device* device, ObjectBuffers& buffers);
    bool CreateConeBuffers(ID3D11Device* device, ObjectBuffers& buffers);
    void ShutdownBuffers();

private:
    std::vector<StaticObject> m_objects;
    ObjectBuffers m_buildingBuffers;
    ObjectBuffers m_treeBuffers;
    ObjectBuffers m_lamppostBuffers;
    ObjectBuffers m_barrierBuffers;
    ObjectBuffers m_coneBuffers;
    bool m_initialized;
};

#endif