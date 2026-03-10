#include "main.h"

PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
PFNGLBINDPROGRAMARBPROC glBindProgramARB;


///////////////////////////////////// INIT GLSL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This function initializes all of our GLSL functions and checks availability.

bool InitGLSL()
{
	char *szGLExtensions = (char*)glGetString(GL_EXTENSIONS);
	if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
	{
		MessageBox(g_hWnd, "GL_ARB_shader_objects extension not supported!", "Error", MB_OK);
		return false;
	}

	if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
	{
		MessageBox(g_hWnd, "GL_ARB_shading_language_100 extension not supported!", "Error", MB_OK);
		return false;
    }

	// Now let's set all of our function pointers for our extension functions
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress("glUniform2fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
	glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
	glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress("glDetachObjectARB");
	glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
	glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)wglGetProcAddress("glProgramLocalParameter4fARB");
	glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");

	return true;
}


///////////////////////////////// LOAD TEXT FILE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
//  This function loads and returns a text file for our shaders

string CShader::LoadTextFile(string strFile)
{
	ifstream fin(strFile.c_str());
	if(!fin)
		return "";

	string strLine = "";
	string strText = "";

	while(getline(fin, strLine))
	{
		strText = strText + "\n" + strLine;
	}

	fin.close();
	return strText;
}

///////////////////////////////// INIT SHADERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::InitShaders(string strVertex, string strFragment)
{

	string strVShader, strFShader;
	if(!strVertex.length() || !strFragment.length())
		return;

	if(m_hVertexShader || m_hFragmentShader || m_hProgramObject)
		Release();

	m_hVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	m_hFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	strVShader = LoadTextFile(strVertex.c_str());
	strFShader = LoadTextFile(strFragment.c_str());
	const char *szVShader = strVShader.c_str();
	const char *szFShader = strFShader.c_str();
	glShaderSourceARB(m_hVertexShader, 1, &szVShader, NULL);
	glShaderSourceARB(m_hFragmentShader, 1, &szFShader, NULL);

	glCompileShaderARB(m_hVertexShader);
	glCompileShaderARB(m_hFragmentShader);
	m_hProgramObject = glCreateProgramObjectARB();

	glAttachObjectARB(m_hProgramObject, m_hVertexShader);
	glAttachObjectARB(m_hProgramObject, m_hFragmentShader);
	glLinkProgramARB(m_hProgramObject);
	glUseProgramObjectARB(m_hProgramObject);
}


///////////////////////////////// GET VARIABLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*


GLint CShader::GetVariable(string strVariable)
{
	if(!m_hProgramObject)
		return -1;

	return glGetUniformLocationARB(m_hProgramObject, strVariable.c_str());
}


///////////////////////////////// RELEASE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CShader::Release()
{
	if(m_hVertexShader)
	{
		glDetachObjectARB(m_hProgramObject, m_hVertexShader);
		glDeleteObjectARB(m_hVertexShader);
		m_hVertexShader = NULL;
	}

	if(m_hFragmentShader)
	{
		glDetachObjectARB(m_hProgramObject, m_hFragmentShader);
		glDeleteObjectARB(m_hFragmentShader);
		m_hFragmentShader = NULL;
	}

	if(m_hProgramObject)
	{
		glDeleteObjectARB(m_hProgramObject);
		m_hProgramObject = NULL;
	}
}
