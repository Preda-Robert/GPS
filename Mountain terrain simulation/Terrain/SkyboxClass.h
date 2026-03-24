#ifndef _SKYBOXCLASS_H_
#define _SKYBOXCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class SkyboxClass
{
private:
  struct VertexType
  {
    XMFLOAT3 position;
  };
public:
  SkyboxClass();
  ~SkyboxClass();
  bool Initialize(ID3D11Device*);
  void Shutdown();
  void Render(ID3D11DeviceContext*);
  int GetIndexCount();
private:
  bool InitializeBuffers(ID3D11Device*);
  void ReleaseBuffers();
  void RenderBuffers(ID3D11DeviceContext*);
private:
  ID3D11Buffer* m_vertexBuffer;
  ID3D11Buffer* m_indexBuffer;
  int m_vertexCount;
  int m_indexCount;
};

#endif