#include "CubemapClass.h"

CubemapClass::CubemapClass()
{
  m_texture = 0;
  m_textureView = 0;
}

CubemapClass::~CubemapClass()
{
}

bool CubemapClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
  char* right, char* left, char* top, char* bottom, char* front, char* back)
{
  D3D11_TEXTURE2D_DESC textureDesc;
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  D3D11_SUBRESOURCE_DATA initData[6];
  unsigned char* faceData[6] = { 0 };
  char* filenames[6] = { right, left, top, bottom, front, back };
  int width = 0, height = 0;
  int faceWidth, faceHeight;

  // Load all 6 faces
  for (int i = 0; i < 6; i++) {
    if (!LoadTarga(filenames[i], faceData[i], faceHeight, faceWidth)) {
      // Cleanup on failure
      for (int j = 0; j <= i; j++) {
        if (faceData[j]) delete[] faceData[j];
      }
      return false;
    }

    // First face sets the size
    if (i == 0) {
      width = faceWidth;
      height = faceHeight;
    }
    else if (faceWidth != width || faceHeight != height) {
      // All faces must be same size
      for (int j = 0; j <= i; j++) {
        if (faceData[j]) delete[] faceData[j];
      }
      return false;
    }

    initData[i].pSysMem = faceData[i];
    initData[i].SysMemPitch = width * 4;
    initData[i].SysMemSlicePitch = 0;
  }

  // Create cubemap texture
  ZeroMemory(&textureDesc, sizeof(textureDesc));
  textureDesc.Width = width;
  textureDesc.Height = height;
  textureDesc.MipLevels = 1;
  textureDesc.ArraySize = 6;
  textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  textureDesc.SampleDesc.Count = 1;
  textureDesc.SampleDesc.Quality = 0;
  textureDesc.Usage = D3D11_USAGE_DEFAULT;
  textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  textureDesc.CPUAccessFlags = 0;
  textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

  if (FAILED(device->CreateTexture2D(&textureDesc, initData, &m_texture))) {
    for (int i = 0; i < 6; i++) {
      if (faceData[i]) delete[] faceData[i];
    }
    return false;
  }

  // Create shader resource view
  ZeroMemory(&srvDesc, sizeof(srvDesc));
  srvDesc.Format = textureDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
  srvDesc.TextureCube.MostDetailedMip = 0;
  srvDesc.TextureCube.MipLevels = 1;

  if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView))) {
    for (int i = 0; i < 6; i++) {
      if (faceData[i]) delete[] faceData[i];
    }
    return false;
  }

  // Cleanup face data
  for (int i = 0; i < 6; i++) {
    if (faceData[i]) {
      delete[] faceData[i];
      faceData[i] = NULL;
    }
  }

  return true;
}

void CubemapClass::Shutdown()
{
  if (m_textureView) {
    m_textureView->Release();
    m_textureView = NULL;
  }

  if (m_texture) {
    m_texture->Release();
    m_texture = NULL;
  }
}

ID3D11ShaderResourceView* CubemapClass::GetTexture()
{
  return m_textureView;
}

bool CubemapClass::LoadTarga(char* filename, unsigned char*& imageData, int& height, int& width)
{
  TargaHeader targaFileHeader;
  FILE* filePtr;
  unsigned char* targaImage;
  unsigned int count;
  int error, bpp, imageSize;

  error = fopen_s(&filePtr, filename, "rb");
  if (error != 0) {
    return false;
  }

  count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
  if (count != 1) {
    fclose(filePtr);
    return false;
  }

  height = (int)targaFileHeader.height;
  width = (int)targaFileHeader.width;
  bpp = (int)targaFileHeader.bpp;

  if (bpp != 32) {
    fclose(filePtr);
    return false;
  }

  imageSize = width * height * 4;

  targaImage = new unsigned char[imageSize];
  if (!targaImage) {
    fclose(filePtr);
    return false;
  }

  count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
  if (count != imageSize) {
    delete[] targaImage;
    fclose(filePtr);
    return false;
  }

  fclose(filePtr);

  imageData = new unsigned char[imageSize];
  if (!imageData) {
    delete[] targaImage;
    return false;
  }

  // TGA is stored bottom-up, BGR format - convert to top-down RGBA
  int index = 0;
  int k = (width * height * 4) - (width * 4);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      imageData[index + 0] = targaImage[k + 2]; // Red
      imageData[index + 1] = targaImage[k + 1]; // Green
      imageData[index + 2] = targaImage[k + 0]; // Blue
      imageData[index + 3] = targaImage[k + 3]; // Alpha

      k += 4;
      index += 4;
    }
    k -= (width * 8);
  }

  delete[] targaImage;

  return true;
}