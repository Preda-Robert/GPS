#ifndef _MAIN_H
#define _MAIN_H

#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library
#include <math.h>
#include <string>
#include <fstream>
using namespace std;

#define GL_FOG_COORDINATE_SOURCE_EXT			0x8450
#define GL_FOG_COORDINATE_EXT					0x8451

// Here we extern our function pointer for volumetric fog positioning
typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);

// These are for our multi-texture defines
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4

#define SAND_TEXTURE_ID 

// These are used to increase the detail texture for our terrain
#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// Here are the multitexture function prototypes
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);

// Here we extern our function pointers for multitexturing capabilities
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;


#define MAX_TEXTURES 1000								// The maximum amount of textures to load

extern UINT g_Texture[MAX_TEXTURES];					// This stores the texture IDs
extern float g_DT;										// Global float that stores the elapsed time between the current and last frame

#define REFLECTION_ID	0
#define REFRACTION_ID	1
#define NORMAL_ID		2
#define DUDVMAP_ID		3
#define DEPTH_ID		4
#define TERRAIN_ID		5
#define DETAIL_ID		6
#define BACK_ID		11		// The texture ID for the back side of the cube
#define FRONT_ID	12		// The texture ID for the front side of the cube
#define BOTTOM_ID	13		// The texture ID for the bottom side of the cube
#define TOP_ID		14		// The texture ID for the top side of the cube
#define LEFT_ID		15		// The texture ID for the left side of the cube
#define RIGHT_ID	16		// The texture ID for the right side of the cube

#define NUM_WATER_TEX 32
#define WATER_START_ID 17
#define WATER_END_ID WATER_START_ID + NUM_WATER_TEX

// We need to define this for glTexParameteri()
#define GL_CLAMP_TO_EDGE	0x812F						


#define SCREEN_WIDTH 1000								
#define SCREEN_HEIGHT 900							
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

extern bool  g_bFullScreen;								// Set full screen as default
extern HWND  g_hWnd;									// This is the handle for the window
extern RECT  g_rRect;									// This holds the window dimensions
extern HDC   g_hDC;										// General HDC - (handle to device context)
extern HGLRC g_hRC;										// General OpenGL_DC - Our Rendering Context for OpenGL
extern HINSTANCE g_hInstance;							// This holds our window hInstance

extern float g_WaterHeight;
extern float g_WaterFlow;
extern float g_WaterUV;

class CVector3
{
public:
	
	
	CVector3() {}
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Here we overload the + operator so we can add vectors together 
	CVector3 operator+(CVector3 vVector)
	{
		// Return the added vectors result.
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Here we overload the - operator so we can subtract vectors 
	CVector3 operator-(CVector3 vVector)
	{
		// Return the subtracted vectors result
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Here we overload the * operator so we can multiply by scalars
	CVector3 operator*(float num)
	{
		// Return the scaled vector
		return CVector3(x * num, y * num, z * num);
	}

	// Here we overload the / operator so we can divide by a scalar
	CVector3 operator/(float num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};



#include "camera.h"
extern CCamera g_Camera;								
#include "CShader.h"
extern CShader g_Shader;
#include "Water.h"
void RenderWorld(bool bRenderCaustics);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop();

bool CreateTexture(UINT &texture, LPSTR strFileName);
void ChangeToFullScreen();
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);
bool bSetupPixelFormat(HDC hdc);
void SizeOpenGLScreen(int width, int height);
void InitializeOpenGL(int width, int height);
void Init(HWND hWnd);
void RenderScene();
void DeInit();

#endif 

