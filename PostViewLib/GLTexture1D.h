// GLTexture1D.h: interface for the CGLTexture1D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLTEXTURE1D_H__1924E4C1_AF77_41B1_8E7D_29CC3ADAA2DB__INCLUDED_)
#define AFX_GLTEXTURE1D_H__1924E4C1_AF77_41B1_8E7D_29CC3ADAA2DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GLTexture1D
{
public:
	GLTexture1D();

	void SetTexture(unsigned char* pb);

	void MakeCurrent();

	int Size();

	unsigned char* GetBytes();

	void Smooth(bool bs);

protected:
	unsigned char m_pb[3*1024];

	int		m_n;
	bool	m_bsmooth;
};

#endif // !defined(AFX_GLTEXTURE1D_H__1924E4C1_AF77_41B1_8E7D_29CC3ADAA2DB__INCLUDED_)
