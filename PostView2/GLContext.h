#pragma once

class CGLView;

class CGLContext
{
public:
	CGLContext(CGLView* pv);
	~CGLContext(void);

public:
	CGLView*	m_pview;
	int	m_x, m_y;
};
