#include "Main.h"

const float kNormalMapScale = 0.25f;
float g_DT = 0.0f;


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; 
	float deltaTime = currentTime - lastTime; 
	float desiredFPS = 1.0f / desiredFrameRate; 

	elapsedTime += deltaTime; 
	lastTime = currentTime; 


	if( elapsedTime > desiredFPS )
	{
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS;

		
		return true;
	}

	return false;
}


/////////////////////////////// LOAD ANIM TEXTURES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadAnimTextures(UINT textureArray[], LPSTR szFileName, int startIndex, int bitmapCount)
{
	char szBuffer[255];
	int i = 0;

	// Don't load anything if a valid file name doesn't exist
	if(!szFileName) return;
	for(i = 0; i < bitmapCount; i++)
	{
		sprintf(szBuffer, "%s%d%d.bmp", szFileName, i / 10, i % 10);
		CreateTexture(textureArray[startIndex + i], szBuffer);
	}
}


///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID)										
{
	unsigned int *pTexture = NULL;											
	pTexture = new unsigned int [size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(unsigned int));	
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	delete [] pTexture;																					
}


///////////////////////////////// RENDER CAUSTICS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderCaustics(float waterHeight, float causticScale)
{
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glEnable(GL_TEXTURE_2D);
	static int startIndex = 0;

	static int frameCount = 0;

	if(frameCount == 5)
	{
		startIndex = ((startIndex + 1) % NUM_WATER_TEX);
		frameCount = 0;
	}

	// Increase the frame count every frame
	frameCount++;

	// Just like we did with the detail texture for the terrain, we
	// scale the caustic texture by our inputted scale value.
	glMatrixMode(GL_TEXTURE);

		// Reset the current matrix and apply our chosen scale value
		glLoadIdentity();
		glScalef(causticScale, causticScale, 1);

	// Leave the texture matrix and put us back in the model view matrix
	glMatrixMode(GL_MODELVIEW);

	// Next, we need to render the terrain with caustic effects, but only
	// the bottom part of the terrain (clip the top part)

	// Create the plane equation for the part we want to keep (bottom)
	double plane[4] = {0.0, -1.0, 0.0, waterHeight};

	// Turn on a clip plane and set the clipping equation
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);



	// Turn clipping off
//	glDisable(GL_CLIP_PLANE0);

	// Reset the texture matrix
	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
}

/////////////////////////// CREATE REFLECTION TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This renders our world to a texture for our reflection

void CreateReflectionTexture(float waterHeight, int textureSize)
{
	
    glViewport(0,0, textureSize, textureSize);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

	// So we don't affect any other objects in the world we push on a new matrix
    glPushMatrix();

		// If our camera is above the water we will render the scene flipped upside down.
		// In order to line up the reflection nicely with the world we have to translate
		// the world to the position of our reflected surface, multiplied by two.
		if(g_Camera.Position().y > waterHeight)
		{
			// Translate the world, then flip it upside down
			glTranslatef(0.0f, waterHeight*2.0f, 0.0f);
			glScalef(1.0, -1.0, 1.0);

			// Since the world is updside down we need to change the culling to FRONT
			glCullFace(GL_FRONT);
		
			double plane[4] = {0.0, 1.0, 0.0, -waterHeight};
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);

			
			RenderWorld(false);

			// Turn clipping off
			glDisable(GL_CLIP_PLANE0);

			// Restore back-face culling
			glCullFace(GL_BACK);
		}
		else
		{
			// If the camera is below the water we don't want to flip the world,
			// but just render it clipped so only the top is drawn.
			double plane[4] = {0.0, 1.0, 0.0, waterHeight};
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
		}

	// Restore the previous matrix
    glPopMatrix();

    // Bind the current scene to our reflection texture
	glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);
}


/////////////////////////// CREATE REFRACTION DEPTH TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This renders our world to a texture to do the reflection and depth calculations

void CreateRefractionDepthTexture(float waterHeight, int textureSize)
{
	
    glViewport(0,0, textureSize, textureSize);

	// Clear the color and depth bits, reset the matrix and position our camera.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    g_Camera.Look();

    glPushMatrix();

		if(g_Camera.Position().y > waterHeight)
		{
			double plane[4] = {0.0, -1.0, 0.0, waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
		}
		// If the camera is below the water, only render the data above the water
		else
		{
			glCullFace(GL_FRONT);
			double plane[4] = {0.0, 1.0, 0.0, -waterHeight}; 
			glEnable(GL_CLIP_PLANE0);
			glClipPlane(GL_CLIP_PLANE0, plane);
			RenderWorld(true);
			glDisable(GL_CLIP_PLANE0);
			glCullFace(GL_BACK);
		}

	// Restore the previous matrix
    glPopMatrix();

    // Bind the current scene to our refraction texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, textureSize, textureSize);

    // Bind the current scene to our depth texture
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, textureSize, textureSize, 0);
}


///////////////////////////////// RENDER WATER \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This function renders the flat-water surface as a quad

void RenderWater(float waterHeight)
{
    // You have reached the last important function of this tutorial!
	// Well, besides the shader files :)  All we are doing here is just
	// turning on all of the texture units for the water and drawing a
	// quad.  The shader code takes care of the rest.  In order for the
	// shader to know which texture is which, we need to tell our shader
	// which textures are assigned to which variables.

	// Turn on the first texture unit and bind the REFLECTION texture
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFLECTION_ID]);

	// Turn on the second texture unit and bind the REFRACTION texture
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[REFRACTION_ID]);

	// Turn on the third texture unit and bind the NORMAL MAP texture
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[NORMAL_ID]);

	// Turn on the fourth texture unit and bind the DUDV MAP texture
    glActiveTextureARB(GL_TEXTURE3_ARB);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_Texture[DUDVMAP_ID]);

	// Turn on the fifth texture unit and bind the DEPTH texture
    glActiveTextureARB(GL_TEXTURE4_ARB);
    glEnable(GL_TEXTURE_2D); 
    glBindTexture(GL_TEXTURE_2D, g_Texture[DEPTH_ID]);

	// Set the variable "reflection" to correspond to the first texture unit
	GLint uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "reflection"); 
	glUniform1iARB(uniform, 0); //second paramter is the texture unit 

	// Set the variable "refraction" to correspond to the second texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "refraction");
	glUniform1iARB(uniform, 1); 

	// Set the variable "normalMap" to correspond to the third texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "normalMap");
	glUniform1iARB(uniform, 2);

	// Set the variable "dudvMap" to correspond to the fourth texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "dudvMap"); 
	glUniform1iARB(uniform, 3);

	// Set the variable "depthMap" to correspond to the fifth texture unit
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "depthMap");
	glUniform1iARB(uniform, 4); 

	
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "waterColor");

	glUniform4fARB(uniform, 0.0f, 1.0f, 1.0f, 1.0f);

	CVector3 lightPos(100.0f, 150.0f, 100.0f);

	// Store the camera position in a variable
	CVector3 vPosition = g_Camera.Position();

	// Give the variable "lightPos" our hard coded light position
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "lightPos");
	glUniform4fARB(uniform, lightPos.x, lightPos.y, lightPos.z, 1.0f); 

	// Give the variable "cameraPos" our camera position
	uniform = glGetUniformLocationARB(g_Shader.GetProgram(), "cameraPos");
	glUniform4fARB(uniform, vPosition.x, vPosition.y, vPosition.z, 1.0f); 
	
	static float move = 0.0f;

	// Use this variable for the normal map and make it slower
	// than the refraction map's speed.  We want the refraction
	// map to be jittery, but not the normal map's waviness.
	float move2 = move * kNormalMapScale;

	// Set the refraction map's UV coordinates to our global g_WaterUV
	float refrUV = g_WaterUV;

	// Set our normal map's UV scale and shrink it by kNormalMapScale
	float normalUV = g_WaterUV * kNormalMapScale;

	// Move the water by our global speed
	move += g_WaterFlow;

    // Draw our huge water quad
    glBegin(GL_QUADS);

		// The back left vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, g_WaterUV);				// Reflection texture				
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, refrUV - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, normalUV + move2);		// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(0.0f, waterHeight, 0.0f);

		// The front left vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, 0.0f, 0.0f);					// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0.0f, 0.0f - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, 0.0f, 0.0f + move2);			// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(0.0f, waterHeight, 1000.0f);

		// The front right vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, 0.0f);				// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, 0.0f - move);			// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, 0.0f + move2);		// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(1000.0f, waterHeight, 1000.0f);

		// The back right vertice for the water
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, g_WaterUV, g_WaterUV);		// Reflection texture
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, refrUV, refrUV - move);		// Refraction texture
		glMultiTexCoord2fARB(GL_TEXTURE2_ARB, normalUV, normalUV + move2);	// Normal map texture
		glMultiTexCoord2fARB(GL_TEXTURE3_ARB, 0, 0);						// DUDV map texture
		glMultiTexCoord2fARB(GL_TEXTURE4_ARB, 0, 0);						// Depth texture
		glVertex3f(1000.0f, waterHeight, 0.0f);

    glEnd();

	// Turn the first multi-texture pass off
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);

	// Turn the second multi-texture pass off
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the third multi-texture pass off
	glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);

	// Turn the fourth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE3_ARB);		
    glDisable(GL_TEXTURE_2D);

	// Turn the fifth multi-texture pass off
	glActiveTextureARB(GL_TEXTURE4_ARB);		
    glDisable(GL_TEXTURE_2D);
}
