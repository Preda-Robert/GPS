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

	if (!m_UserInterface ||
		!m_Camera ||
		!m_Light ||
		!m_Position ||
		!m_Frustum ||
		!m_SkyDome ||
		!m_Skybox ||     
		!m_Cubemap ||     
		!m_Terrain ||
		!m_UserInterface->Initialize(direct3D, screenHeight, screenWidth)
		) {
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->Render();
	m_Camera->RenderBaseViewMatrix();

	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-0.5f, -1.0f, -0.5f);

	m_Position->SetPosition(16.0f, 78.0f, 565.0f);
	m_Position->SetRotation(13.0f, 89.0f, 0.0f);

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
	if (m_Terrain) {
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = NULL;
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

bool ZoneClass::Render(D3DClass* direct3D, ShaderManagerClass* shaderManager, TextureManagerClass* textureManager)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;
	XMFLOAT3 cameraPosition;
	bool result;
	int i;

	m_Camera->Render();

	direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	direct3D->GetOrthoMatrix(orthoMatrix);

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

	for (i = 0; i < m_Terrain->GetCellCount(); ++i) {
		if (m_Terrain->RenderCell(direct3D->GetDeviceContext(), i, m_Frustum)) {
			result = shaderManager->RenderTerrainShader(
				direct3D->GetDeviceContext(),
				m_Terrain->GetCellIndexCount(i),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				textureManager->GetTexture(0),  
				textureManager->GetTexture(1), 
				textureManager->GetTexture(2),  
				textureManager->GetTexture(3),  
				textureManager->GetTexture(4),  
				textureManager->GetTexture(5), 
				m_Light->GetDirection(),
				m_Light->GetDiffuseColor()
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
