#include "ZoneClass.h"

ZoneClass::ZoneClass()
{
	m_UserInterface = NULL;
	m_Camera = NULL;
	m_Light = NULL;
	m_Position = NULL;
	m_Frustum = NULL;
	m_SkyDome = NULL;
	m_Skybox = NULL;
	m_Cubemap = NULL;
	m_Terrain = NULL;
	m_ShadowMap = NULL;
	m_StreetLights = NULL;
	for (int i = 0; i < 8; i++) {
		m_StreetLightShadowMaps[i] = NULL;
	}
	m_StreetCircuit = NULL;
	m_StaticObjects = NULL;
	m_ControllableObject = NULL;
	m_MovingObjects = NULL;
	m_displayUI = false; // F1
	m_wireFrame = false; // F2
	m_play = false; // F3
	m_cellLines = false; // F4
	m_heightLocked = false; // F5
}

ZoneClass::~ZoneClass()
{
}

bool ZoneClass::Initialize(D3DClass* direct3D, HWND hwnd, int screenWidth, int screenHeight, float screenDepth)
{
	m_UserInterface = new UserInterfaceClass();
	m_Camera = new CameraClass();
	m_Light = new LightClass();
	m_Position = new PositionClass();
	m_Frustum = new FrustumClass();
	m_SkyDome = new SkyDomeClass();
	m_Skybox = new SkyboxClass();
	m_Cubemap = new CubemapClass();
	m_Terrain = new TerrainClass();
	m_ShadowMap = new ShadowMapClass();
	m_StreetLights = new StreetLightClass();

	m_StreetCircuit = new StreetCircuitClass();
	if (!m_StreetCircuit || !m_StreetCircuit->Initialize(direct3D->GetDevice(),
		StreetCircuitClass::OVAL,  
		128.0f,   
		400.0f, 
		200.0f,   
		120.0f,   
		15.0f))   
	{
		return false;
	}

	m_StaticObjects = new StaticObjectsClass();
	if (!m_StaticObjects || !m_StaticObjects->Initialize(direct3D->GetDevice()))
	{
		return false;
	}
	float offset = 272.0f;
	
	m_StaticObjects->AddObject(StaticObjectsClass::BUILDING, 50.0f, 15.0f, offset + 50.0f, 15.0f, 25.0f, 15.0f, 0.0f);
	m_StaticObjects->AddObject(StaticObjectsClass::BUILDING, 200.0f, 15.0f, offset + 50.0f, 20.0f, 35.0f, 12.0f, 0.3f);
	m_StaticObjects->AddObject(StaticObjectsClass::BUILDING, 50.0f, 15.0f, offset + 200.0f, 18.0f, 30.0f, 18.0f, -0.2f);

	m_StaticObjects->AddObject(StaticObjectsClass::TREE, 30.0f, 15.0f, offset + 100.0f, 5.0f, 8.0f, 5.0f, 0.0f);
	m_StaticObjects->AddObject(StaticObjectsClass::TREE, 230.0f, 15.0f, offset + 150.0f, 6.0f, 10.0f, 6.0f, 0.5f);
	m_StaticObjects->AddObject(StaticObjectsClass::TREE, 100.0f, 15.0f, offset + 30.0f, 4.0f, 7.0f, 4.0f, 0.0f);

	m_StaticObjects->AddObject(StaticObjectsClass::LAMPPOST, 60.0f, 15.0f, offset + 128.0f, 2.0f, 15.0f, 2.0f, 0.0f);
	m_StaticObjects->AddObject(StaticObjectsClass::LAMPPOST, 196.0f, 15.0f, offset + 128.0f, 2.0f, 15.0f, 2.0f, 0.0f);

	m_StaticObjects->AddObject(StaticObjectsClass::BARRIER, 128.0f, 15.0f, offset + 40.0f, 8.0f, 2.0f, 2.0f, 0.0f);
	m_StaticObjects->AddObject(StaticObjectsClass::BARRIER, 128.0f, 15.0f, offset + 216.0f, 8.0f, 2.0f, 2.0f, 0.0f);

	m_StaticObjects->AddObject(StaticObjectsClass::CONE, 70.0f, 15.0f, offset + 80.0f, 3.0f, 3.0f, 3.0f, 0.0f);
	m_StaticObjects->AddObject(StaticObjectsClass::CONE, 186.0f, 15.0f, offset + 176.0f, 3.0f, 3.0f, 3.0f, 0.0f);

	m_ControllableObject = new ControllableObjectClass();
	if (!m_ControllableObject || !m_ControllableObject->Initialize(direct3D->GetDevice(), ControllableObjectClass::CAR))
	{
		return false;
	}
	m_ControllableObject->SetPosition(128.0f, 15.0f, offset + 128.0f);

	m_MovingObjects = new MovingObjectsClass();
	if (!m_MovingObjects || !m_MovingObjects->Initialize(direct3D->GetDevice()))
	{
		return false;
	}
	
	m_MovingObjects->AddRandomMover(100.0f, 17.0f, offset + 100.0f, 2.0f, MovingObjectsClass::CUBE, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 15.0f);
	m_MovingObjects->AddRandomMover(150.0f, 17.0f, offset + 150.0f, 1.5f, MovingObjectsClass::CUBE, XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), 20.0f);
	m_MovingObjects->AddRandomMover(80.0f, 17.0f, offset + 180.0f, 2.5f, MovingObjectsClass::CUBE, XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), 12.0f);


	m_MovingObjects->AddBouncingObject(120.0f, 17.0f, offset + 80.0f, 2.0f, MovingObjectsClass::SPHERE, XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), 10.0f, 8.0f);
	m_MovingObjects->AddBouncingObject(140.0f, 17.0f, offset + 90.0f, 1.8f, MovingObjectsClass::SPHERE, XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), 15.0f, 12.0f);

	m_MovingObjects->AddCircularMover(128.0f, 20.0f, offset + 128.0f, 20.0f, 2.0f, MovingObjectsClass::CUBE, XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), 1.0f);

	if (!m_UserInterface ||
		!m_Camera ||
		!m_Light ||
		!m_Position ||
		!m_Frustum ||
		!m_SkyDome ||
		!m_Skybox ||
		!m_Cubemap ||
		!m_Terrain ||
		!m_ShadowMap ||
		!m_StreetLights ||
		!m_UserInterface->Initialize(direct3D, screenHeight, screenWidth) ||
		!m_ShadowMap->Initialize(direct3D->GetDevice(), 2048, 2048) ||
		!m_StreetLights->Initialize(direct3D->GetDevice())
		) {
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);
	

	float sceneCenter = 512.0f;
	m_Light->SetPosition(sceneCenter + 200.0f, 400.0f, sceneCenter + 200.0f);
	m_Light->SetLookAt(sceneCenter, 0.0f, sceneCenter);
	m_Light->GenerateViewMatrix();
	m_Light->GenerateProjectionMatrix(1000.0f, 1.0f);

	XMFLOAT4 streetlightColor = XMFLOAT4(1.0f, 0.9f, 0.7f, 1.0f);
	float lightRange = 50.0f;
	float lightIntensity = 2.0f;
	
	m_StreetLights->AddStreetLight(XMFLOAT3(60.0f, 15.0f + 15.0f, offset + 128.0f), streetlightColor, lightRange, lightIntensity);
	m_StreetLights->AddStreetLight(XMFLOAT3(196.0f, 15.0f + 15.0f, offset + 128.0f), streetlightColor, lightRange, lightIntensity);
	
	for (int i = 0; i < m_StreetLights->GetLightCount() && i < 8; i++) {
		m_StreetLightShadowMaps[i] = new ShadowMapClass();
		if (!m_StreetLightShadowMaps[i]->Initialize(direct3D->GetDevice(), 1024, 1024)) {
			return false;
		}
	}

	//m_Position->SetPosition(16.0f, 78.0f, 565.0f);
	//m_Position->SetRotation(13.0f, 89.0f, 0.0f);
	m_Position->SetPosition(128.0f, 10.0f, 128.0f);
	m_Position->SetRotation(20.0f, 0.0f, 0.0f);
	m_Frustum->Initialize(screenDepth);

	if (!m_SkyDome->Initialize(direct3D->GetDevice()) ||
		!m_Skybox->Initialize(direct3D->GetDevice()) ||
		!m_Cubemap->Initialize(direct3D->GetDevice(), direct3D->GetDeviceContext(),
			"data/skybox/right.tga",
			"data/skybox/left.tga",
			"data/skybox/top.tga",
			"data/skybox/bottom.tga",
			"data/skybox/front.tga",
			"data/skybox/back.tga") ||
		!m_Terrain->Initialize(direct3D->GetDevice(), "data/Setup.txt")
		) {
		return false;
	}

	return true;
}

void ZoneClass::Shutdown()
{
	if (m_MovingObjects)
	{
		m_MovingObjects->Shutdown();
		delete m_MovingObjects;
		m_MovingObjects = NULL;
	}

	if (m_ControllableObject)
	{
		m_ControllableObject->Shutdown();
		delete m_ControllableObject;
		m_ControllableObject = NULL;
	}

	if (m_StaticObjects)
	{
		m_StaticObjects->Shutdown();
		delete m_StaticObjects;
		m_StaticObjects = NULL;
	}

	if (m_StreetCircuit)
	{
		m_StreetCircuit->Shutdown();
		delete m_StreetCircuit;
		m_StreetCircuit = NULL;
	}
	if (m_Terrain) {
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = NULL;
	}

	if (m_ShadowMap) {
		m_ShadowMap->Shutdown();
		delete m_ShadowMap;
		m_ShadowMap = NULL;
	}

	for (int i = 0; i < 8; i++) {
		if (m_StreetLightShadowMaps[i]) {
			m_StreetLightShadowMaps[i]->Shutdown();
			delete m_StreetLightShadowMaps[i];
			m_StreetLightShadowMaps[i] = NULL;
		}
	}

	if (m_StreetLights) {
		m_StreetLights->Shutdown();
		delete m_StreetLights;
		m_StreetLights = NULL;
	}

	if (m_Cubemap) {
		m_Cubemap->Shutdown();
		delete m_Cubemap;
		m_Cubemap = NULL;
	}

	if (m_Skybox) {
		m_Skybox->Shutdown();
		delete m_Skybox;
		m_Skybox = NULL;
	}

	if (m_SkyDome) {
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = NULL;
	}

	if (m_Frustum) {
		delete m_Frustum;
		m_Frustum = NULL;
	}

	if (m_Position) {
		delete m_Position;
		m_Position = NULL;
	}

	if (m_Light) {
		delete m_Light;
		m_Light = NULL;
	}

	if (m_Camera) {
		delete m_Camera;
		m_Camera = NULL;
	}

	if (m_UserInterface) {
		m_UserInterface->Shutdown();
		delete m_UserInterface;
		m_UserInterface = NULL;
	}

	return;
}

bool ZoneClass::Frame(D3DClass* direct3D, InputClass* input, ShaderManagerClass* shaderManager, TextureManagerClass* textureManager, float frameTime, int fps)
{
	float posX, posY, posZ, rotX, rotY, rotZ, height;

	HandleMovementInput(input, frameTime);
	HandleControllableObjectInput(input, frameTime);

	m_MovingObjects->Update(frameTime, 30.0f, 220.0f, 300.0f, 480.0f);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	if (m_play) {
		PushedF3Button(frameTime);
	}

	if (!m_UserInterface->Frame(direct3D->GetDeviceContext(), fps, posX, posY, posZ, rotX, rotY, rotZ, m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor())) {
		return false;
	}

	m_Terrain->Frame();

	if (m_heightLocked && m_Terrain->GetHeightAtPosition(posX, posZ, height)) {
		m_Position->SetPosition(posX, height + 1.0f, posZ);
		m_Camera->SetPosition(posX, height + 1.0f, posZ);
	}

	if (!Render(direct3D, shaderManager, textureManager)) {
		return false;
	}

	return true;
}

void ZoneClass::HandleMovementInput(InputClass* input, float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	m_Position->SetFrameTime(frameTime);

	keyDown = input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = input->IsUpPressed();
	m_Position->MoveForward(keyDown);

	keyDown = input->IsDownPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = input->IsWPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = input->IsXPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = input->IsAPressed();
	m_Position->MoveLeftward(keyDown);

	keyDown = input->IsDPressed();
	m_Position->MoveRightward(keyDown);

	keyDown = input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	if (input->IsF1Toggled()) {
		m_displayUI = !m_displayUI;
	}

	if (input->IsF2Toggled()) {
		m_wireFrame = !m_wireFrame;
	}

	if (input->IsF3Toggled()) {
		m_play = !m_play;
	}

	if (input->IsF4Toggled()) {
		m_cellLines = !m_cellLines;
	}

	if (input->IsF5Toggled()) {
		m_heightLocked = !m_heightLocked;
	}

	return;
}

void ZoneClass::HandleControllableObjectInput(InputClass* input, float frameTime)
{
	bool keyDown;
	float objX, objY, objZ;
	float terrainHeight;

	m_ControllableObject->SetFrameTime(frameTime);

	keyDown = input->IsIPressed();
	m_ControllableObject->MoveForward(keyDown);

	keyDown = input->IsKPressed();
	m_ControllableObject->MoveBackward(keyDown);

	keyDown = input->IsJPressed();
	m_ControllableObject->TurnLeft(keyDown);

	keyDown = input->IsLPressed();
	m_ControllableObject->TurnRight(keyDown);

	m_ControllableObject->GetPosition(objX, objY, objZ);
	
	if (!m_Terrain->GetHeightAtPosition(objX, objZ, terrainHeight))
	{
		terrainHeight = objY;
	}

	m_ControllableObject->Update(m_StaticObjects, terrainHeight);

	return;
}

bool ZoneClass::RenderShadowMap(D3DClass* direct3D, ShaderManagerClass* shaderManager)
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;
	
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	direct3D->GetWorldMatrix(worldMatrix);
	
	m_ShadowMap->SetRenderTarget(direct3D->GetDeviceContext());
	m_ShadowMap->ClearRenderTarget(direct3D->GetDeviceContext());
	
	for (int i = 0; i < m_Terrain->GetCellCount(); ++i) {
		m_Terrain->RenderCellForShadow(direct3D->GetDeviceContext(), i);
		shaderManager->RenderDepthShader(
			direct3D->GetDeviceContext(),
			m_Terrain->GetCellIndexCount(i),
			worldMatrix,
			lightViewMatrix,
			lightProjectionMatrix
		);
	}
	
	for (int i = 0; i < m_StaticObjects->GetObjectCount(); i++) {
		XMMATRIX objectWorld = m_StaticObjects->GetObjectWorldMatrix(i);
		m_StaticObjects->Render(direct3D->GetDeviceContext(), i);
		shaderManager->RenderDepthShader(
			direct3D->GetDeviceContext(),
			m_StaticObjects->GetIndexCount(i),
			objectWorld,
			lightViewMatrix,
			lightProjectionMatrix
		);
	}
	
	direct3D->SetBackBufferRenderTarget();
	direct3D->ResetViewport();
	
	return true;
}

bool ZoneClass::Render(D3DClass* direct3D, ShaderManagerClass* shaderManager, TextureManagerClass* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	XMFLOAT3 cameraPosition;
	bool result;
	int i;

	RenderShadowMap(direct3D, shaderManager);

	m_Camera->Render();

	direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	cameraPosition = m_Camera->GetPosition();

	m_Frustum->ConstructFrustum(projectionMatrix, viewMatrix);

	direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	direct3D->TurnOffCulling();
	direct3D->TurnZBufferOff();

	worldMatrix = XMMatrixScaling(500.0f, 500.0f, 500.0f);

	m_Skybox->Render(direct3D->GetDeviceContext());

	result = shaderManager->RenderSkyboxShader(
		direct3D->GetDeviceContext(),
		m_Skybox->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_Cubemap->GetTexture()
	);

	if (!result) {
		return false;
	}

	direct3D->GetWorldMatrix(worldMatrix);
	direct3D->TurnZBufferOn();
	direct3D->TurnOnCulling();

	if (m_wireFrame) {
		direct3D->EnableWireframe();
	}

	TerrainShaderClass::PointLightBufferType pointLightBuffer;
	int lightCount = m_StreetLights->GetLightCount();
	pointLightBuffer.numLights = lightCount;
	pointLightBuffer.padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (int li = 0; li < lightCount && li < MAX_POINT_LIGHTS; li++) {
		StreetLightData* light = m_StreetLights->GetLightData(li);
		pointLightBuffer.positions[li] = XMFLOAT4(light->position.x, light->position.y, light->position.z, light->range);
		pointLightBuffer.colors[li] = XMFLOAT4(light->color.x, light->color.y, light->color.z, light->intensity);
		pointLightBuffer.attenuation[li] = XMFLOAT4(light->constantAttenuation, light->linearAttenuation, 
			light->quadraticAttenuation, light->enabled ? 1.0f : 0.0f);
	}

	for (i = 0; i < m_Terrain->GetCellCount(); ++i) {
		if (m_Terrain->RenderCell(direct3D->GetDeviceContext(), i, m_Frustum)) {
			result = shaderManager->RenderTerrainShader(
				direct3D->GetDeviceContext(),
				m_Terrain->GetCellIndexCount(i),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				lightViewMatrix,
				lightProjectionMatrix,
				textureManager->GetTexture(0),
				textureManager->GetTexture(1),
				textureManager->GetTexture(2),
				textureManager->GetTexture(3),
				textureManager->GetTexture(4),
				textureManager->GetTexture(5),
				m_ShadowMap->GetShaderResourceView(),
				m_Light->GetDirection(),
				m_Light->GetDiffuseColor(),
				&pointLightBuffer
			);

			if (!result) {
				return false;
			}

			if (m_cellLines) {
				m_Terrain->RenderCellLines(direct3D->GetDeviceContext(), i);

				if (!shaderManager->RenderColorShader(direct3D->GetDeviceContext(),
					m_Terrain->GetCellLinesIndexCount(i), worldMatrix, viewMatrix, projectionMatrix)) {
					return false;
				}
			}
		}
	}

	// Render Street Circuit
	m_StreetCircuit->Render(direct3D->GetDeviceContext());
	result = shaderManager->RenderObjectShader(
		direct3D->GetDeviceContext(),
		m_StreetCircuit->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		textureManager->GetTexture(6),  // Circuit texture (asphalt)
		m_Light->GetDirection(),
		m_Light->GetDiffuseColor()
	);
	if (!result) return false;

	// Render Static Objects
	for (int i = 0; i < m_StaticObjects->GetObjectCount(); i++)
	{
		XMMATRIX objectWorld = m_StaticObjects->GetObjectWorldMatrix(i);

		m_StaticObjects->Render(direct3D->GetDeviceContext(), i);

		// Choose texture based on object type
		int textureIndex = 7; // Default building texture
		switch (m_StaticObjects->GetObjectType(i))
		{
		case StaticObjectsClass::BUILDING:  textureIndex = 7; break;
		case StaticObjectsClass::TREE:      textureIndex = 8; break;
		case StaticObjectsClass::LAMPPOST:  textureIndex = 9; break;
		case StaticObjectsClass::BARRIER:   textureIndex = 7; break;
		case StaticObjectsClass::CONE:      textureIndex = 7; break;
		}

		result = shaderManager->RenderObjectShader(
			direct3D->GetDeviceContext(),
			m_StaticObjects->GetIndexCount(i),
			objectWorld,
			viewMatrix,
			projectionMatrix,
			textureManager->GetTexture(textureIndex),
			m_Light->GetDirection(),
			m_Light->GetDiffuseColor()
		);
		if (!result) return false;
	}

	{
		XMMATRIX ctrlObjWorld = m_ControllableObject->GetWorldMatrix();
		m_ControllableObject->Render(direct3D->GetDeviceContext());
		
		
		int textureIndex = 7;
		if (m_ControllableObject->GetObjectType() == ControllableObjectClass::CAR)
		{
			textureIndex = 7; 
		}
		else
		{
			textureIndex = 9;
		}

		result = shaderManager->RenderObjectShader(
			direct3D->GetDeviceContext(),
			m_ControllableObject->GetIndexCount(),
			ctrlObjWorld,
			viewMatrix,
			projectionMatrix,
			textureManager->GetTexture(textureIndex),
			m_Light->GetDirection(),
			m_Light->GetDiffuseColor()
		);
		if (!result) return false;
	}

	for (int i = 0; i < m_MovingObjects->GetObjectCount(); i++)
	{
		XMMATRIX movingObjWorld = m_MovingObjects->GetObjectWorldMatrix(i);
		m_MovingObjects->Render(direct3D->GetDeviceContext(), i);

		result = shaderManager->RenderObjectShader(
			direct3D->GetDeviceContext(),
			m_MovingObjects->GetIndexCount(i),
			movingObjWorld,
			viewMatrix,
			projectionMatrix,
			textureManager->GetTexture(7), 
			m_Light->GetDirection(),
			m_Light->GetDiffuseColor()
		);
		if (!result) return false;
	}

	if (m_wireFrame) {
		direct3D->DisableWireframe();
	}

	if (!m_UserInterface->UpdateRenderCounts(direct3D->GetDeviceContext(),
		m_Terrain->GetRenderCount(), m_Terrain->GetCellsDrawn(), m_Terrain->GetCellsCulled())) {
		return false;
	}

	if (m_displayUI && !m_UserInterface->Render(direct3D, shaderManager, worldMatrix, baseViewMatrix, orthoMatrix)) {
		return false;
	}

	direct3D->EndScene();

	return true;
}

// 0 - 257 : 129
void ZoneClass::PushedF3Button(float frameTime)
{
	XMFLOAT3 dir;

	dir = m_Light->GetDirection();

	if (dir.x >= 360) {
		dir.x -= 360;
	}

	if (dir.y >= 360) {
		dir.y -= 360;
	}

	if (dir.z >= 360) {
		dir.z -= 360;
	}

	dir.z -= frameTime / 5;

	m_Light->SetDirection(dir.x, dir.y, dir.z);

	//XMFLOAT4 tempColor;
	//tempColor = m_SkyDome->GetApexColor();
	//tempColor.x -= frameTime / 5; // R
	//tempColor.y -= frameTime / 5; // G
	//tempColor.z -= frameTime / 5; // B
	//m_SkyDome->SetApexColor(tempColor);

	return;
}
