#include "UserInterfaceClass.h"

UserInterfaceClass::UserInterfaceClass()
{
	m_font1 = 0;
	m_fpsString = 0;
	m_videoStrings = 0;
	m_positionStrings = 0;
	m_skyColorStrings = 0;
	m_renderCountStrings = 0;
	m_MiniMap = 0;
	m_previousFps = -1;
	m_previousPosition[5] = { -1 };
	m_previousSkyColor[7] = { -1 };
	m_skyColorTypeStrings;
}

UserInterfaceClass::~UserInterfaceClass()
{
}

bool UserInterfaceClass::Initialize(D3DClass* Direct3D, int screenHeight, int screenWidth)
{
	char videoCard[128];
	int videoMemory;
	char videoString[144];
	char memoryString[32];
	char tempString[16];

	m_font1 = new FontClass();
	m_fpsString = new TextClass();
	m_videoStrings = new TextClass[2];
	m_positionStrings = new TextClass[6];
	m_skyColorTypeStrings = new TextClass[2];
	m_skyColorStrings = new TextClass[8];
	m_renderCountStrings = new TextClass[3];
	m_MiniMap = new MiniMapClass();

	if (!m_font1 ||
		!m_fpsString ||
		!m_videoStrings ||
		!m_positionStrings ||
		!m_skyColorTypeStrings ||
		!m_skyColorStrings ||
		!m_renderCountStrings ||
		!m_MiniMap ||
		!m_font1->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), "data/font/font01.txt", "data/font/font01.tga", 32.0f, 3) ||
		!m_fpsString->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "Fps: 0", 10, -50, 0.0f, 1.0f, 0.0f)
	) {
		return false;
	}

	Direct3D->GetVideoCardInfo(videoCard, videoMemory);
	strcpy_s(videoString, "Video Card: ");
	strcat_s(videoString, videoCard);

	_itoa_s(videoMemory, tempString, 10);

	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");


	if (!m_videoStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 256, false, m_font1, videoString, 10, -10, 1.0f, 1.0f, 1.0f) ||
		!m_videoStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_font1, memoryString, 10, -30, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "X: 0", 10, -310, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "Y: 0", 10, -330, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[2].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "Z: 0", 10, -350, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[3].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "rX: 0", 10, -370, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[4].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "rY: 0", 10, -390, 1.0f, 1.0f, 1.0f) ||
		!m_positionStrings[5].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "rZ: 0", 10, -410, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorTypeStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "ApexColor", 10, -270, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorTypeStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "CenterColor", 10, -380, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "R: 0", 10, -290, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "G: 0", 10, -310, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[2].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "B: 0", 10, -330, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[3].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "A: 0", 10, -350, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[4].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "R: 0", 10, -400, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[5].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "G: 0", 10, -420, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[6].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "B: 0", 10, -440, 1.0f, 1.0f, 1.0f) ||
		!m_skyColorStrings[7].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 16, false, m_font1, "A: 0", 10, -460, 1.0f, 1.0f, 1.0f) ||
		!m_renderCountStrings[0].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_font1, "Polys Drawn: 0", 10, -490, 1.0f, 1.0f, 1.0f) ||
		!m_renderCountStrings[1].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_font1, "Cells Drawn: 0", 10, -510, 1.0f, 1.0f, 1.0f) ||
		!m_renderCountStrings[2].Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 32, false, m_font1, "Cells Culled: 0", 10, -530, 1.0f, 1.0f, 1.0f) ||
		!m_MiniMap->Initialize(Direct3D->GetDevice(), Direct3D->GetDeviceContext(), screenWidth, screenHeight, 1025, 1025)
	) {
		return false;
	}

	return true;
}

void UserInterfaceClass::Shutdown()
{
	if (m_MiniMap) {
		m_MiniMap->Shutdown();
		delete m_MiniMap;
		m_MiniMap = NULL;
	}

	if (m_renderCountStrings) {
		m_renderCountStrings[0].Shutdown();
		m_renderCountStrings[1].Shutdown();
		m_renderCountStrings[2].Shutdown();
		delete[] m_renderCountStrings;
		m_renderCountStrings = NULL;
	}

	if (m_positionStrings) {
		m_positionStrings[0].Shutdown();
		m_positionStrings[1].Shutdown();
		m_positionStrings[2].Shutdown();
		m_positionStrings[3].Shutdown();
		m_positionStrings[4].Shutdown();
		m_positionStrings[5].Shutdown();
		delete[] m_positionStrings;
		m_positionStrings = NULL;
	}

	if (m_skyColorStrings) {
		m_skyColorStrings[0].Shutdown();
		m_skyColorStrings[1].Shutdown();
		m_skyColorStrings[2].Shutdown();
		m_skyColorStrings[3].Shutdown();
		m_skyColorStrings[4].Shutdown();
		m_skyColorStrings[5].Shutdown();
		m_skyColorStrings[6].Shutdown();
		m_skyColorStrings[7].Shutdown();
		delete[] m_skyColorStrings;
		m_skyColorStrings = NULL;
	}

	if (m_skyColorTypeStrings) {
		m_skyColorTypeStrings[0].Shutdown();
		m_skyColorTypeStrings[1].Shutdown();
		delete[] m_skyColorTypeStrings;
		m_skyColorTypeStrings = NULL;
	}

	if (m_videoStrings) {
		m_videoStrings[0].Shutdown();
		m_videoStrings[1].Shutdown();
		delete[] m_videoStrings;
		m_videoStrings = NULL;
	}

	if (m_fpsString) {
		m_fpsString->Shutdown();
		delete m_fpsString;
		m_fpsString = NULL;
	}

	if (m_font1) {
		m_font1->Shutdown();
		delete m_font1;
		m_font1 = NULL;
	}

	return;
}

bool UserInterfaceClass::Frame(ID3D11DeviceContext* deviceContext, int fps, float posX, float posY, float posZ, float rotX, float rotY, float rotZ, XMFLOAT4 apexColor, XMFLOAT4 centerColor)
{
	if (!UpdateFpsString(deviceContext, fps) ||
		!UpdatePositionStrings(deviceContext, posX, posY, posZ, rotX, rotY, rotZ) ||
		!UpdateSkyColorStrings(deviceContext, apexColor, centerColor)
	) {
		return false;
	}

	m_MiniMap->PositionUpdate(posX, posZ);

	return true;
}

bool UserInterfaceClass::Render(D3DClass* Direct3D, ShaderManagerClass* shaderManager, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX orthoMatrix)
{
	int i;

	Direct3D->TurnZBufferOff();
	Direct3D->EnableAlphaBlending();

	m_fpsString->Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	m_videoStrings[0].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	m_videoStrings[1].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());

	for (i = 0; i < 6; ++i) {
		m_positionStrings[i].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	}

	m_skyColorTypeStrings[0].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	m_skyColorTypeStrings[1].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());

	for (i = 0; i < 8; ++i) {
		m_skyColorStrings[i].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	}

	for (i = 0; i < 3; ++i) {
		m_renderCountStrings[i].Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix, m_font1->GetTexture());
	}

	Direct3D->DisableAlphaBlending();

	if (!m_MiniMap->Render(Direct3D->GetDeviceContext(), shaderManager, worldMatrix, viewMatrix, orthoMatrix)) {
		return false;
	}

	Direct3D->TurnZBufferOn();

	return true;
}

bool UserInterfaceClass::UpdateRenderCounts(ID3D11DeviceContext* deviceContext, int renderCount, int nodesDrawn, int nodeCulled)
{
	char tempString[32];
	char finalString[32];

	_itoa_s(renderCount, tempString, 10);

	strcpy_s(finalString, "Polys Drawn: ");
	strcat_s(finalString, tempString);

	if (!m_renderCountStrings[0].UpdateSentence(deviceContext, m_font1, finalString, 10, -490, 1.0f, 1.0f, 1.0f)) {
		return false;
	}

	_itoa_s(nodesDrawn, tempString, 10);

	strcpy_s(finalString, "Cells Drawn: ");
	strcat_s(finalString, tempString);

	if (!m_renderCountStrings[1].UpdateSentence(deviceContext, m_font1, finalString, 10, -510, 1.0f, 1.0f, 1.0f)) {
		return false;
	}

	_itoa_s(nodeCulled, tempString, 10);

	strcpy_s(finalString, "Cells Culled: ");
	strcat_s(finalString, tempString);

	if (!m_renderCountStrings[2].UpdateSentence(deviceContext, m_font1, finalString, 10, -530, 1.0f, 1.0f, 1.0f)) {
		return false;
	}

	return true;
}

bool UserInterfaceClass::UpdateFpsString(ID3D11DeviceContext* deviceContext, int fps)
{
	char tempString[16];
	char finalString[16];
	float red, green, blue;

	if (m_previousFps == fps) {
		return true;
	}

	m_previousFps = fps;

	if (fps > 99999) {
		fps = 99999;
	}

	_itoa_s(fps, tempString, 10);

	strcpy_s(finalString, "Fps: ");
	strcat_s(finalString, tempString);

	if (fps >= 60) {
		red = 0.0f;
		green = 1.0f;
		blue = 0.0;
	}

	if (fps < 60) {
		red = 1.0f;
		green = 1.0f;
		blue = 0.0;
	}

	if (fps < 30) {
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	if (!m_fpsString->UpdateSentence(deviceContext, m_font1, finalString, 10, -50, red, green, blue)) {
		return false;
	}

	return true;
}

bool UserInterfaceClass::UpdatePositionStrings(ID3D11DeviceContext* deviceContext, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	int positionX, positionY, positionZ, rotationX, rotationY, rotationZ;
	char tempString[16];
	char finalString[16];

	positionX = (int)posX;
	positionY = (int)posY;
	positionZ = (int)posZ;
	rotationX = (int)rotX;
	rotationY = (int)rotY;
	rotationZ = (int)rotZ;

	if (positionX != m_previousPosition[0]) {
		m_previousPosition[0] = positionX;

		_itoa_s(positionX, tempString, 10);
		strcpy_s(finalString, "X: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[0].UpdateSentence(deviceContext, m_font1, finalString, 10, -100, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (positionY != m_previousPosition[1]) {
		m_previousPosition[1] = positionY;

		_itoa_s(positionY, tempString, 10);
		strcpy_s(finalString, "Y: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[1].UpdateSentence(deviceContext, m_font1, finalString, 10, -120, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (positionZ != m_previousPosition[2]) {
		m_previousPosition[2] = positionZ;

		_itoa_s(positionZ, tempString, 10);
		strcpy_s(finalString, "Z: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[2].UpdateSentence(deviceContext, m_font1, finalString, 10, -140, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (rotationX != m_previousPosition[3]) {
		m_previousPosition[3] = rotationX;

		_itoa_s(rotationX, tempString, 10);
		strcpy_s(finalString, "rX: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[3].UpdateSentence(deviceContext, m_font1, finalString, 10, -180, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (rotationY != m_previousPosition[4]) {
		m_previousPosition[4] = rotationY;

		_itoa_s(rotationY, tempString, 10);
		strcpy_s(finalString, "rY: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[4].UpdateSentence(deviceContext, m_font1, finalString, 10, -200, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (rotationZ != m_previousPosition[5]) {
		m_previousPosition[5] = rotationZ;

		_itoa_s(rotationZ, tempString, 10);
		strcpy_s(finalString, "rZ: ");
		strcat_s(finalString, tempString);

		if (!m_positionStrings[5].UpdateSentence(deviceContext, m_font1, finalString, 10, -220, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	return true;
}

bool UserInterfaceClass::UpdateSkyColorStrings(ID3D11DeviceContext* deviceContext, XMFLOAT4 apexColor, XMFLOAT4 centerColor)
{
	float apexR, apexG, apexB, apexA;
	float centerR, centerG, centerB, centerA;
	char tempString[16];
	char finalString[16];

	apexR = apexColor.x;
	apexG = apexColor.y;
	apexB = apexColor.z;
	apexA = apexColor.w;

	centerR = centerColor.x;
	centerG = centerColor.y;
	centerB = centerColor.z;
	centerA = centerColor.w;

	if (apexR != m_previousSkyColor[0]) {
		m_previousSkyColor[0] = apexR;

		sprintf_s(tempString, "%.3f", apexR);
		strcpy_s(finalString, "R: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[0].UpdateSentence(deviceContext, m_font1, finalString, 10, -290, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (apexG != m_previousSkyColor[1]) {
		m_previousSkyColor[1] = apexG;

		sprintf_s(tempString, "%.3f", apexG);
		strcpy_s(finalString, "G: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[1].UpdateSentence(deviceContext, m_font1, finalString, 10, -310, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (apexB != m_previousSkyColor[2]) {
		m_previousSkyColor[2] = apexB;

		sprintf_s(tempString, "%.3f", apexB);
		strcpy_s(finalString, "B: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[2].UpdateSentence(deviceContext, m_font1, finalString, 10, -330, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (apexA != m_previousSkyColor[3]) {
		m_previousSkyColor[3] = apexA;

		sprintf_s(tempString, "%.3f", apexA);
		strcpy_s(finalString, "A: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[3].UpdateSentence(deviceContext, m_font1, finalString, 10, -350, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (centerR != m_previousSkyColor[4]) {
		m_previousSkyColor[4] = centerR;

		sprintf_s(tempString, "%.3f", centerR);
		strcpy_s(finalString, "R: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[4].UpdateSentence(deviceContext, m_font1, finalString, 10, -400, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (centerG != m_previousSkyColor[5]) {
		m_previousSkyColor[5] = centerG;

		sprintf_s(tempString, "%.3f", centerG);
		strcpy_s(finalString, "G: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[5].UpdateSentence(deviceContext, m_font1, finalString, 10, -420, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (centerB != m_previousSkyColor[6]) {
		m_previousSkyColor[6] = centerB;

		sprintf_s(tempString, "%.3f", centerB);
		strcpy_s(finalString, "B: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[6].UpdateSentence(deviceContext, m_font1, finalString, 10, -440, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	if (centerA != m_previousSkyColor[7]) {
		m_previousSkyColor[7] = centerA;

		sprintf_s(tempString, "%.3f", centerA);
		strcpy_s(finalString, "A: ");
		strcat_s(finalString, tempString);

		if (!m_skyColorStrings[7].UpdateSentence(deviceContext, m_font1, finalString, 10, -460, 1.0f, 1.0f, 1.0f)) {
			return false;
		}
	}

	return true;
}
