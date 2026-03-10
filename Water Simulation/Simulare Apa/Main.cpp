#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#include "main.h"								

bool  g_bFullScreen = true;						
HWND  g_hWnd;									
RECT  g_rRect;									
HDC   g_hDC;									
HGLRC g_hRC;									
HINSTANCE g_hInstance;							

/*
CONTROLS:

 The camera is moved by the mouse and the arrow keys (also w, a, s, d).  
 F1 - Makes the water appear farther away by shrinking the water texture
 F2 - Makes the water appear closer by enlarging the water texture
 F3 - Makes the water move faster
 F4 - Makes the water slower

*/


CShader g_Shader;
int g_TextureSize = 300;
float g_WaterHeight = 30.0f;
float g_WaterUV = 5.0f;

// The speed of the water flow
float g_WaterFlow = 0.0015f;
const float kCausticScale = 4.0f;
PFNGLFOGCOORDFEXTPROC glFogCoordfEXT = NULL;
float g_FogDepth = 20.0f; 

PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;

bool g_bDetail = true;
int g_DetailScale = 5;
CCamera g_Camera;										
bool  g_bRenderMode = true;						

// This holds the texture info by an ID
UINT g_Texture[MAX_TEXTURES] = {0};		


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
// This function initializes the application

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		MessageBox(g_hWnd, "Your current setup does not support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

	glFogCoordfEXT	= (PFNGLFOGCOORDFEXTPROC) wglGetProcAddress("glFogCoordfEXT");

	
	if(!glFogCoordfEXT)
	{
		MessageBox(g_hWnd, "Your current setup does not support volumetric fog", "Error", MB_OK);
		PostQuitMessage(0);
	}

	g_Camera.PositionCamera( 475, 52, 301,  474, 52, 300,  0, 1, 0);

	
	CreateRenderTexture(g_Texture, g_TextureSize, 3, GL_RGB, REFLECTION_ID);
	CreateRenderTexture(g_Texture, g_TextureSize, 3, GL_RGB, REFRACTION_ID);
	CreateRenderTexture(g_Texture, g_TextureSize, 1, GL_DEPTH_COMPONENT, DEPTH_ID);

	CreateTexture(g_Texture[NORMAL_ID],	 "Textures\\normalmap.bmp");// Load the normal map water texture
	CreateTexture(g_Texture[DUDVMAP_ID], "Textures\\dudvmap.bmp");	// Load the dudv map water texture
	CreateTexture(g_Texture[TERRAIN_ID], "Textures\\Terrain.bmp");	// Load the terrain texture
	CreateTexture(g_Texture[DETAIL_ID],	 "Textures\\Detail.bmp");	// Load the detail texture for the terrain
	CreateTexture(g_Texture[BACK_ID],	 "Textures\\Back.bmp");		// Load the Sky box Back texture
	CreateTexture(g_Texture[FRONT_ID],	 "Textures\\Front.bmp");	// Load the Sky box Front texture
	CreateTexture(g_Texture[BOTTOM_ID],  "Textures\\Bottom.bmp");	// Load the Sky box Bottom texture
	CreateTexture(g_Texture[TOP_ID],	 "Textures\\Top.bmp");		// Load the Sky box Top texture
	CreateTexture(g_Texture[LEFT_ID],	 "Textures\\Left.bmp");		// Load the Sky box Left texture
	CreateTexture(g_Texture[RIGHT_ID],	 "Textures\\Right.bmp");	// Load the Sky box Right texture
	
	LoadAnimTextures(g_Texture, "Textures\\caust", WATER_START_ID, NUM_WATER_TEX);
	InitGLSL();
	g_Shader.InitShaders("water.vert", "water.frag");
	g_Shader.TurnOff();
}

///////////////////////////////// RENDER WORLD \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
// This function renders the terrain and caustic effects

void RenderWorld(bool bRenderCaustics)
{
	
	if(bRenderCaustics)
		RenderCaustics(g_WaterHeight, kCausticScale);

}

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
// This function renders the entire scene.

void RenderScene()
{
	
	CreateReflectionTexture(g_WaterHeight, g_TextureSize);
    CreateRefractionDepthTexture(g_WaterHeight, g_TextureSize);

  
    glViewport(0,0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

	// Get the current position of the camera
	CVector3 vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;

	g_Camera.Look();

    RenderWorld(true);
	g_Shader.TurnOn();

	glDisable(GL_CULL_FACE);
	RenderWater(g_WaterHeight);
	glEnable(GL_CULL_FACE);

	g_Shader.TurnOff();
	SwapBuffers(g_hDC);	
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This function handles the main loop

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											
		{ 		
			if(AnimateNextFrame(40))					
			{
				g_Camera.Update();						
				RenderScene();							
			}
			else
			{
				Sleep(1);								
			}
        } 
	}
	
	DeInit();											
	return(msg.wParam);									
}


///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This function handles the window messages.

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;


	case WM_LBUTTONDOWN:
	
		g_bDetail = !g_bDetail;
		break;

	case WM_RBUTTONDOWN:								// If the left mouse button was clicked
		
		g_bRenderMode = !g_bRenderMode;

		// Change the rendering mode to and from lines or triangles
		if(g_bRenderMode) 				
		{
			// Render the triangles in fill mode		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// Render the triangles in wire frame mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_SPACE:								// Check if we hit the SPACE bar

				// Times the current scale value by 2 and loop when it gets to 128
				g_DetailScale = (g_DetailScale * 2) % 128;

				// If the scale value is 0, set it to 1 again
				if(g_DetailScale == 0)
					g_DetailScale = 1;
				break;

			case VK_ADD:								// Check if we hit the + key

				g_FogDepth += 1;						// Increase the fog height

				if(g_FogDepth > 200)					// Make sure we don't go past 200
					g_FogDepth = 200;

				break;

			case VK_SUBTRACT:							// Check if we hit the - key

				g_FogDepth -= 1;						// Decrease the fog height

				if(g_FogDepth < 0)						// Make sure we don't go below 0
					g_FogDepth = 0;

				break;

			// To control the water's realism we added F1-F4 keys to change the
			// water's speed and texture size.  F1 is my favorite :)

			case VK_F1:									// Check if we hit the F1 key

				g_WaterUV += 1.0f;						// Increase the water UV texture
				break;
			
			case VK_F2:									// Check if we hit the F2 key

				g_WaterUV -= 1.0f;						// Increase the water UV texture

				if(g_WaterUV < 0.0f)					// If water's UV is below zero, reset it.
					g_WaterUV = 0.0f;
				break;

			case VK_F3:									// Check if we hit the F3 key

				g_WaterFlow += 0.00003f;				// Increase the water's speed
				break;

			
			case VK_F4:									// Check if we hit the F4 key

				g_WaterFlow -= 0.00003f;				// Decrease the water's speed

				if(g_WaterFlow < 0.0f)					// If speed is below zero, reset it.
					g_WaterFlow = 0.0f;
				break;

		}
		break;
 
    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Return the default
}


