#include "main.h"

///////////////////////////////// CREATE TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CreateTexture(GLuint &textureID, LPTSTR szFileName)                          
{
	HBITMAP hBMP;                                                                 
	BITMAP  bitmap;																  

	glGenTextures(1, &textureID);                                                
	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

	if (!hBMP)                                                                   
		  return FALSE;                                                           

	GetObject(hBMP, sizeof(bitmap), &bitmap);                                    								 

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);                                        
	glBindTexture(GL_TEXTURE_2D, textureID);                                      
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);			 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);			  
	glTexImage2D(GL_TEXTURE_2D, 0, 3, bitmap.bmWidth, bitmap.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap.bmBits);
	DeleteObject(hBMP);                                                           

	return TRUE;                                                                  
}


///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//						This changes the screen to FULL SCREEN

void ChangeToFullScreen()
{
	DEVMODE dmSettings = {0};							

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;				// Selected Screen Width
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;			// Selected Screen Height
	dmSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}


///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//			This function creates a window, but doesn't have a message loop


HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));				// Init the size of the class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Regular drawing capabilities
	wndclass.lpfnWndProc = WinProc;						// Pass our function pointer as the window procedure
	wndclass.hInstance = hInstance;						// Assign our hInstance
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// General icon
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);		// An arrow for the cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window
	wndclass.lpszClassName = "GameTutorials";			// Assign the class name

	RegisterClass(&wndclass);							// Register the class
	
	if(bFullScreen && !dwStyle) 						// Check if we wanted full screen mode
	{													// Set the window properties for full screen mode
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();							// Go to full screen
		ShowCursor(FALSE);								// Hide the cursor
	}
	else if(!dwStyle)									// Assign styles to the window depending on the choice
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;							// Assign our global hInstance to the window's hInstance

	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0
	rWindow.right	= width;							// Set Right Value To Requested Width
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height

	AdjustWindowRect( &rWindow, dwStyle, false);		// Adjust Window To True Requested Size

														// Create the window
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;								

	ShowWindow(hWnd, SW_SHOWNORMAL);					
	UpdateWindow(hWnd);									

	SetFocus(hWnd);										

	return hWnd;
}

///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//					This function sets the pixel format for OpenGL.

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
    pfd.nVersion = 1;									
														
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					
    pfd.iPixelType = PFD_TYPE_RGBA;						
    pfd.cColorBits = SCREEN_DEPTH;						
    pfd.cDepthBits = SCREEN_DEPTH;						
    pfd.cAccumBits = 0;									
    pfd.cStencilBits = 0;								
 
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
	// This sets the pixel format that we extracted from above
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
}


//////////////////////////// SIZE OPENGL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SizeOpenGLScreen(int width, int height)			// Initialize The GL Window
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						
														

	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 0.9f ,4000.0f);

	glMatrixMode(GL_MODELVIEW);							
	glLoadIdentity();									
}


///////////////////////////////// INITIALIZE GL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*


void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);								
														
    if (!bSetupPixelFormat(g_hDC))						
        PostQuitMessage (0);							

    g_hRC = wglCreateContext(g_hDC);					
    wglMakeCurrent(g_hDC, g_hRC);						

	SizeOpenGLScreen(width, height);					
}


///////////////////////////////// DE INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//	This function cleans up and then posts a quit message to the window

void DeInit()
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						
		wglDeleteContext(g_hRC);						
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						
		
	if(g_bFullScreen)									
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);								
	}

	UnregisterClass("GameTutorials", g_hInstance);		

	PostQuitMessage (0);								
}


///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//		This function handles registering and creating the window.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;
	g_bFullScreen = false;
	hWnd = CreateMyWindow("Simulare apa", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	if(hWnd == NULL) return TRUE;
	Init(hWnd);													
	return (int)MainLoop();						
}

