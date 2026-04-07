#ifndef _STREETLIGHTCLASS_H_
#define _STREETLIGHTCLASS_H_

#include <D3D11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

// Maximum number of streetlights that can be rendered
#define MAX_STREETLIGHTS 8

struct StreetLightData
{
	XMFLOAT3 position;
	float range;
	XMFLOAT4 color;
	float intensity;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	bool enabled;
	XMFLOAT3 padding;
};

class StreetLightClass
{
public:
	StreetLightClass();
	~StreetLightClass();

	bool Initialize(ID3D11Device*);
	void Shutdown();

	// Add a streetlight at the specified position
	int AddStreetLight(XMFLOAT3 position, XMFLOAT4 color, float range, float intensity = 1.0f);
	
	// Remove a streetlight
	void RemoveStreetLight(int index);
	
	// Update streetlight properties
	void SetPosition(int index, XMFLOAT3 position);
	void SetColor(int index, XMFLOAT4 color);
	void SetRange(int index, float range);
	void SetIntensity(int index, float intensity);
	void SetAttenuation(int index, float constant, float linear, float quadratic);
	void SetEnabled(int index, bool enabled);
	
	// Getters
	int GetLightCount() const;
	StreetLightData* GetLightData(int index);
	XMFLOAT3 GetPosition(int index) const;
	XMFLOAT4 GetColor(int index) const;
	float GetRange(int index) const;
	float GetIntensity(int index) const;
	bool IsEnabled(int index) const;
	
	// Get all lights for shader
	void GetAllLights(StreetLightData* outLights, int& outCount) const;

private:
	std::vector<StreetLightData> m_lights;
};

#endif
