#pragma once
#include <QWidget>

namespace Ui {
	class CTimePanel;
};

class CMainWindow;

class CTimePanel : public QWidget
{
	Q_OBJECT

public:
	CTimePanel(CMainWindow* wnd, QWidget* parent = 0);

	void Update(bool reset = false);

	void SetCurrentTime(int n);

private slots:
	void on_timer_pointClicked(int n);

private:
	CMainWindow*	m_wnd;
	Ui::CTimePanel*	ui;
};
