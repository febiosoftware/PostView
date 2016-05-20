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
	// This function must be overloaded by derived classes.
	// When reset is true the panel should update all its content
	// When reset is false, only values should be updated
	virtual void Update(bool breset);

protected:
	CMainWindow*	m_wnd;
};
