#include "InputClass.h"

InputClass::InputClass()
{
	m_screenHeight = NULL;
	m_screenWidth = NULL;
	m_directInput = NULL;
	m_keyboard = NULL;
	m_mouse = NULL;
	m_F1_released = true;
	m_F2_released = true;
	m_F3_released = true;
	m_F4_released = true;
	m_F5_released = true;
	m_mouseX = 0;
	m_mouseY = 0;

	m_keyboardState;
	m_mouseState;
}

InputClass::~InputClass()
{
}

bool InputClass::initialze(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	if (FAILED(DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL)) ||
		FAILED(m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL)) ||
		FAILED(m_keyboard->SetDataFormat(&c_dfDIKeyboard)) ||
		FAILED(m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)) ||
		FAILED(m_keyboard->Acquire()) ||
		FAILED(m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL)) ||
		FAILED(m_mouse->SetDataFormat(&c_dfDIMouse)) ||
		FAILED(m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)) ||
		FAILED(m_mouse->Acquire())
	) {
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = NULL;
	}

	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = NULL;
	}

	if (m_directInput) {
		m_directInput->Release();
		m_directInput = NULL;
	}

	return;
}

bool InputClass::Frame()
{
	if (!ReadKeyboard()) {
		return false;
	}

	if (!ReadMouse()) {
		return false;
	}

	ProcessInput();

	return true;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;

	return;
}

bool InputClass::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsLeftPressed()
{
	if (m_keyboardState[DIK_LEFT] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsRightPressed()
{
	if (m_keyboardState[DIK_RIGHT] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsUpPressed()
{
	if (m_keyboardState[DIK_UP] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsDownPressed()
{
	if (m_keyboardState[DIK_DOWN] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsWPressed()
{
	if (m_keyboardState[DIK_W] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsXPressed()
{
	if (m_keyboardState[DIK_X] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsAPressed()
{
	if (m_keyboardState[DIK_A] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsDPressed()
{
	if (m_keyboardState[DIK_D] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsPgUpPressed()
{
	if (m_keyboardState[DIK_PGUP] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsPgDownPressed()
{
	if (m_keyboardState[DIK_PGDN] & 0x80) {
		return true;
	}

	return false;
}

bool InputClass::IsF1Toggled()
{
	if (m_keyboardState[DIK_F1] & 0x80) {
		if (m_F1_released) {
			m_F1_released = false;

			return true;
		}
	}
	else {
		m_F1_released = true;
	}

	return false;
}

bool InputClass::IsF2Toggled()
{
	if (m_keyboardState[DIK_F2] & 0x80) {
		if (m_F2_released) {
			m_F2_released = false;

			return true;
		}
	}
	else {
		m_F2_released = true;
	}

	return false;
}

bool InputClass::IsF3Toggled()
{
	if (m_keyboardState[DIK_F3] & 0x80) {
		if (m_F3_released) {
			m_F3_released = false;

			return true;
		}
	}
	else {
		m_F3_released = true;
	}

	return false;
}

bool InputClass::IsF4Toggled()
{
	if (m_keyboardState[DIK_F4] & 0x80) {
		if (m_F4_released) {
			m_F4_released = false;

			return true;
		}
	}
	else {
		m_F4_released = true;
	}

	return false;
}

bool InputClass::IsF5Toggled()
{
	if (m_keyboardState[DIK_F5] & 0x80) {
		if (m_F5_released) {
			m_F5_released = false;

			return true;
		}
	}
	else {
		m_F5_released = true;
	}

	return false;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);

	if (FAILED(result)) {
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			m_keyboard->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

	if (FAILED(result)) {
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			m_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0) {
		m_mouseX = 0;
	}

	if (m_mouseY < 0) {
		m_mouseY = 0;
	}

	if (m_mouseX > m_screenWidth) {
		m_mouseX = m_screenWidth;
	}

	if (m_mouseY > m_screenHeight) {
		m_mouseY = m_screenHeight;
	}

	return;
}
