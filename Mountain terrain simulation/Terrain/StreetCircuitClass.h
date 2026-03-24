#ifndef _STREETCIRCUITCLASS_H_
#define _STREETCIRCUITCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class StreetCircuitClass
{
private:
    struct VertexType
    {
        XMFLOAT3 position;
        XMFLOAT2 texture;
        XMFLOAT3 normal;
    };

public:
    enum CircuitType { CIRCULAR, OVAL, RECTANGULAR };

    StreetCircuitClass();
    ~StreetCircuitClass();

    bool Initialize(ID3D11Device*, CircuitType type, float centerX, float centerZ, float width, float height, float roadWidth);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();
    XMFLOAT3 GetPosition();

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    void BuildCircularCircuit(float centerX, float centerZ, float radius, float roadWidth);
    void BuildOvalCircuit(float centerX, float centerZ, float width, float height, float roadWidth);
    void BuildRectangularCircuit(float centerX, float centerZ, float width, float height, float roadWidth);

private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    int m_vertexCount, m_indexCount;

    VertexType* m_vertices;
    unsigned long* m_indices;

    CircuitType m_circuitType;
    float m_centerX, m_centerZ;
    float m_width, m_height, m_roadWidth;
};

#endif