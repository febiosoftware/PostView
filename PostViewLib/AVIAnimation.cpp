#include "stdafx.h"
#include "AVIAnimation.h"
#ifdef WIN32

CAVIAnimation::CAVIAnimation()
{
	m_pfile = NULL;
	m_pavi = NULL;
	m_pavicmp = NULL;
	m_nsample = 0;

	m_cx = m_cy = 0;
}

void CAVIAnimation::Close()
{
	if (m_pavi) AVIStreamRelease(m_pavi); 
	if (m_pavicmp) AVIStreamRelease(m_pavi); 
	if (m_pfile) AVIFileRelease(m_pfile); 

	m_pfile = 0;
	m_pavi = 0;
	m_pavicmp = 0;
	m_nsample = 0;
	m_cx = m_cy = 0;
}

int CAVIAnimation::Create(const char* szfile, int cx, int cy, float fps)
{
	// make sure the animation is closed
	Close();

	// fill the stream info
	ZeroMemory(&m_ai, sizeof(AVISTREAMINFO));
	m_ai.fccType = streamtypeVIDEO;			// type of stream (=VIDEO)
	m_ai.dwScale = 100;						// time scale factor (rate/scale = fps)
	m_ai.dwRate = (int)(m_ai.dwScale*fps);	// playback rate
	m_ai.rcFrame.left   = 0;
	m_ai.rcFrame.top    = 0;
	m_ai.rcFrame.right  = cx;
	m_ai.rcFrame.bottom = cy;
	m_ai.dwSuggestedBufferSize = 0; //cx*cy*3;

	// create a new avifile
	HRESULT hr = AVIFileOpenA(&m_pfile, szfile, OF_CREATE | OF_WRITE, NULL);
	if (hr != 0) return FALSE;

	// create a new video stream
	hr = AVIFileCreateStream(m_pfile, &m_pavi, &m_ai);
	if (hr != 0)
	{
		AVIFileRelease(m_pfile);
		m_pfile = 0;
		return FALSE;
	}

	// create a compressed video stream
	AVICOMPRESSOPTIONS ops = {0};
	AVICOMPRESSOPTIONS* pops = &ops;
	if (AVISaveOptions(NULL, 0, 1, &m_pavi, &pops))
	{
		hr = AVIMakeCompressedStream(&m_pavicmp, m_pavi, &ops, NULL);
		if (hr != 0)
		{
			Close();
			return FALSE;
		}
	}
	else
	{
		Close();
		return FALSE;
	}

	// set the file format header info
	BITMAPINFO *pbmi = (BITMAPINFO*) &m_bmi;
	ZeroMemory(&pbmi->bmiHeader, sizeof(BITMAPINFOHEADER));	

	pbmi->bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth		= cx;
	pbmi->bmiHeader.biHeight	= cy;
	pbmi->bmiHeader.biBitCount	= 24;
	pbmi->bmiHeader.biPlanes	= 1;

	return TRUE;
}

int CAVIAnimation::Write(CRGBImage& im)
{
	// make sure there is a file and a stream
	if ((m_pfile == NULL) || (m_pavi == NULL)) return FALSE;

	HRESULT hr;

	// if this is the first image set the AVI stream format
	if (m_nsample == 0)
	{
		hr = AVIStreamSetFormat(m_pavicmp, 0, (LPVOID) &m_bmi, sizeof(BITMAPINFOHEADER));
		if (hr != 0)
		{
			Close();
			return FALSE;
		}

		m_cx = im.Width();
		m_cy = im.Height();
	}
	else // else make sure the format is still the same
	{
		if ((m_cx != im.Width()) || (m_cy != im.Height()))
		{
			Close();
			return FALSE;
		}
	}

	// write the image data to the stream
	hr = AVIStreamWrite(m_pavicmp, m_nsample, 1, im.GetBytes(), im.Size(), AVIIF_KEYFRAME, NULL, NULL);
	if (hr != 0)
	{
		// close the file
		Close();
		return FALSE;
	}

	m_nsample++;

	return TRUE;
}
#endif
