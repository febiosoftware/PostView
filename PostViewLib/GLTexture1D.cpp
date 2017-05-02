// GLTexture1D.cpp: implementation of the CGLTexture1D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef WIN32
#include <Windows.h>
#include <gl/GL.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif
#ifdef LINUX
#include <GL/gl.h>
#endif
#include "GLTexture1D.h"

GLTexture1D::GLTexture1D()
{
	m_n = 1024; 
	for (int i=0; i<3*m_n; i++) m_pb[i] = 0;
}

GLTexture1D::GLTexture1D(const GLTexture1D& tex)
{
	m_n = tex.m_n;
	for (int i = 0; i<3 * m_n; i++) m_pb[i] = tex.m_pb[i];
}

void GLTexture1D::SetTexture(unsigned char* pb)
{ 
	for (int i=0; i<3*m_n; i++) m_pb[i] = pb[i]; 
}

void GLTexture1D::MakeCurrent()
{ 
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //(m_bsmooth ? GL_LINEAR : GL_NEAREST));
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //(m_bsmooth ? GL_LINEAR : GL_NEAREST));
		
	glTexImage1D(GL_TEXTURE_1D, 0, 3, m_n, 0, GL_RGB, GL_UNSIGNED_BYTE, m_pb); 
};

int GLTexture1D::Size()
{
	return m_n; 
}

unsigned char* GLTexture1D::GetBytes() 
{ 
	return m_pb; 
}
