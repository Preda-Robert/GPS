#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_ShaderManager = 0;
	m_TextureManager = 0;
	m_Timer = 0;
	m_Fps = 0;
	m_Zone = 0;
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_Input = new InputClass();
	m_Direct3D = new D3DClass();
	m_ShaderManager = new ShaderManagerClass();
	m_TextureManager = new TextureManagerClass();
	m_Timer = new TimerClass();
	m_Fps = new FpsClass();
	m_Zone = new ZoneClass();

	if (!m_Input ||
		!m_Direct3D ||
		!m_ShaderManager ||
		!m_TextureManager ||
		!m_Timer ||
		!m_Fps ||
		!m_Zone ||
		!m_Input->initialze(hinstance, hwnd, screenWidth, screenHeight) ||
		!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR) ||
		!m_ShaderManager->Initialize(m_Direct3D->GetDevice(), hwnd) ||
		!m_TextureManager->Initialize(10) ||
		!m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/textures/rock01d.tga", 0) ||
		!m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/textures/rock01n.tga", 1) ||
		!m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/textures/snow01n.tga", 2) ||
		!m_TextureManager->LoadTexture(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "data/textures/distance01n.tga", 3) ||
		!m_Timer->Initialize() ||
		!m_Zone->Initialize(m_Direct3D, hwnd, screenWidth, screenHeight, SCREEN_DEPTH)
	) {
		return false;
	}

	m_Fps->Initialize();

	return true;
}

void ApplicationClass::Shutdown()
{
	if (m_Zone) {
		m_Zone->Shutdown();
		delete m_Zone;
		m_Zone = NULL;
	}

	if (m_Fps) {
		delete m_Fps;
		m_Fps = NULL;
	}

	if (m_Timer) {
		delete m_Timer;
		m_Timer = NULL;
	}

	if (m_TextureManager) {
		m_TextureManager->Shutdown();
		delete m_TextureManager;
		m_TextureManager = 0;
	}

	if (m_ShaderManager) {
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = NULL;
	}

	if (m_Direct3D) {
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = NULL;
	}

	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = NULL;
	}

	return;
}

bool ApplicationClass::Frame()
{
	m_Fps->Frame();
	m_Timer->Frame();

	if (!m_Input->Frame() ||
		m_Input->IsEscapePressed() ||
		!m_Zone->Frame(m_Direct3D, m_Input, m_ShaderManager, m_TextureManager, m_Timer->GetTime(), m_Fps->GetFps())
	) {
		return false;
	}

	return true;
}
