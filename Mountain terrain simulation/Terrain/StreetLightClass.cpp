#include "StreetLightClass.h"

StreetLightClass::StreetLightClass()
{
}

StreetLightClass::~StreetLightClass()
{
}

bool StreetLightClass::Initialize(ID3D11Device* device)
{
	m_lights.clear();
	return true;
}

void StreetLightClass::Shutdown()
{
	m_lights.clear();
}

int StreetLightClass::AddStreetLight(XMFLOAT3 position, XMFLOAT4 color, float range, float intensity)
{
	if (m_lights.size() >= MAX_STREETLIGHTS) {
		return -1;  // Maximum lights reached
	}

	StreetLightData light;
	light.position = position;
	light.color = color;
	light.range = range;
	light.intensity = intensity;
	light.constantAttenuation = 1.0f;
	light.linearAttenuation = 0.09f;
	light.quadraticAttenuation = 0.032f;
	light.enabled = true;
	light.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_lights.push_back(light);
	return (int)(m_lights.size() - 1);
}

void StreetLightClass::RemoveStreetLight(int index)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights.erase(m_lights.begin() + index);
	}
}

void StreetLightClass::SetPosition(int index, XMFLOAT3 position)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].position = position;
	}
}

void StreetLightClass::SetColor(int index, XMFLOAT4 color)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].color = color;
	}
}

void StreetLightClass::SetRange(int index, float range)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].range = range;
	}
}

void StreetLightClass::SetIntensity(int index, float intensity)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].intensity = intensity;
	}
}

void StreetLightClass::SetAttenuation(int index, float constant, float linear, float quadratic)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].constantAttenuation = constant;
		m_lights[index].linearAttenuation = linear;
		m_lights[index].quadraticAttenuation = quadratic;
	}
}

void StreetLightClass::SetEnabled(int index, bool enabled)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		m_lights[index].enabled = enabled;
	}
}

int StreetLightClass::GetLightCount() const
{
	return (int)m_lights.size();
}

StreetLightData* StreetLightClass::GetLightData(int index)
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return &m_lights[index];
	}
	return nullptr;
}

XMFLOAT3 StreetLightClass::GetPosition(int index) const
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return m_lights[index].position;
	}
	return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

XMFLOAT4 StreetLightClass::GetColor(int index) const
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return m_lights[index].color;
	}
	return XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
}

float StreetLightClass::GetRange(int index) const
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return m_lights[index].range;
	}
	return 0.0f;
}

float StreetLightClass::GetIntensity(int index) const
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return m_lights[index].intensity;
	}
	return 0.0f;
}

bool StreetLightClass::IsEnabled(int index) const
{
	if (index >= 0 && index < (int)m_lights.size()) {
		return m_lights[index].enabled;
	}
	return false;
}

void StreetLightClass::GetAllLights(StreetLightData* outLights, int& outCount) const
{
	outCount = (int)m_lights.size();
	for (int i = 0; i < outCount && i < MAX_STREETLIGHTS; i++) {
		outLights[i] = m_lights[i];
	}
}
