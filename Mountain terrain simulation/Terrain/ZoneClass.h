#ifndef _ZONECLASS_H_
#define _ZONECLASS_H_

#include "D3DClass.h"
#include "InputClass.h"
#include "ShaderManagerClass.h"
#include "TextureManagerClass.h"
#include "TimerClass.h"
#include "UserInterfaceClass.h"
#include "CameraClass.h"
#include "LightClass.h"
#include "PositionClass.h"
#include "FrustumClass.h"
#include "SkyDomeClass.h"
#include "SkyboxClass.h"      
#include "CubemapClass.h"    
#include "TerrainClass.h"
#include "StreetCircuitClass.h"
#include "StaticObjectsClass.h"
#include "ShadowMapClass.h"
#include "StreetLightClass.h"
#include "ControllableObjectClass.h"

class ZoneClass
{
public:
	ZoneClass();
	~ZoneClass();
	bool Initialize(D3DClass*, HWND, int, int, float);
	void Shutdown();
	bool Frame(D3DClass*, InputClass*, ShaderManagerClass*, TextureManagerClass*, float, int);

private:
	void HandleMovementInput(InputClass*, float);
	void HandleControllableObjectInput(InputClass*, float);
	void PushedF3Button(float);
	bool RenderShadowMap(D3DClass*, ShaderManagerClass*);
	bool Render(D3DClass*, ShaderManagerClass*, TextureManagerClass*);

private:
	UserInterfaceClass* m_UserInterface;
	CameraClass* m_Camera;
	LightClass* m_Light;
	PositionClass* m_Position;
	FrustumClass* m_Frustum;
	SkyDomeClass* m_SkyDome;
	SkyboxClass* m_Skybox;
	CubemapClass* m_Cubemap;
	TerrainClass* m_Terrain;
	ShadowMapClass* m_ShadowMap;
	StreetLightClass* m_StreetLights;
	ShadowMapClass* m_StreetLightShadowMaps[8];  // One shadow map per streetlight
	bool m_displayUI;
	bool m_wireFrame;
	bool m_play;
	bool m_cellLines;
	bool m_heightLocked;
	StreetCircuitClass* m_StreetCircuit;
	StaticObjectsClass* m_StaticObjects;
	ControllableObjectClass* m_ControllableObject;
};

#endif