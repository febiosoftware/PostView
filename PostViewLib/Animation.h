// Animation.h: interface for the CAnimation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANIMATION_H__AF14AE44_13BD_4E97_B6F6_80E8009C9276__INCLUDED_)
#define AFX_ANIMATION_H__AF14AE44_13BD_4E97_B6F6_80E8009C9276__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class QImage;

//-----------------------------------------------------------------------------
//! Base class for creating animation
class CAnimation  
{
public:
	CAnimation();
	virtual ~CAnimation();

public:
	virtual int Create(const char* szfile, int cx, int cy, float fps = 10.f) = 0;
	virtual int Write(QImage& im) = 0;
	virtual bool IsValid() = 0;
	virtual void Close();
	virtual int Frames() = 0;
};

#endif // !defined(AFX_ANIMATION_H__AF14AE44_13BD_4E97_B6F6_80E8009C9276__INCLUDED_)
