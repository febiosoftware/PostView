#pragma once
#include "math3d.h"

class CGLView;

class CGLContext
{
public:
	CGLContext(CGLView* pv);
	~CGLContext(void);

public:
	CGLView*	m_pview;
	int			m_x, m_y;
	quat4f		m_q;

	bool		m_showMesh;
	bool		m_showOutline;
	float		m_springThick;
};
