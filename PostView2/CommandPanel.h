#pragma once
#include <QWidget>

//-----------------------------------------------------------------------------
class CMainWindow;

//-----------------------------------------------------------------------------
class CCommandPanel : public QWidget
{
public:
	CCommandPanel(CMainWindow* pwnd, QWidget* parent = 0);

public:
	virtual void Update();

protected:
	CMainWindow*	m_wnd;
};
