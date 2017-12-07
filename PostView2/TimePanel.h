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

	void SetRange(int nmin, int nmax);

private slots:
	void on_timer_pointClicked(int n);
	void on_timer_rangeChanged(int nmin, int nmax);

private:
	CMainWindow*	m_wnd;
	Ui::CTimePanel*	ui;
};
