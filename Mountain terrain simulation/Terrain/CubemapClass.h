#ifndef _CUBEMAPCLASS_H_
#define _CUBEMAPCLASS_H_

#include <d3d11.h>
#include <stdio.h>

class CubemapClass
{
  struct TargaHeader
  {
    unsigned char data1[12];
    unsigned short width;
    unsigned short height;
    unsigned char bpp;
    unsigned char data2;
  };
public:
  CubemapClass();
  ~CubemapClass();

  // Initialize with 6 face textures: +X, -X, +Y, -Y, +Z, -Z
  bool Initialize(ID3D11Device*, ID3D11DeviceContext*,
    char* right, char* left, char* top, char* bottom, char* front, char* back);
  void Shutdown();
  ID3D11ShaderResourceView* GetTexture();

private:
  bool LoadTarga(char*, unsigned char*&, int&, int&);

private:
  ID3D11Texture2D* m_texture;
  ID3D11ShaderResourceView* m_textureView;
};

#endif