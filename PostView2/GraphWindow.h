#pragma once
#include <QMainWindow>

class CMainWindow;

namespace Ui {
	class CGraphWindow;
};

class CGraphWindow : public QMainWindow
{
	Q_OBJECT

public:
	CGraphWindow(CMainWindow* wnd);

	void Update();

private:
	CMainWindow*		m_wnd;
	Ui::CGraphWindow*	ui;
};
