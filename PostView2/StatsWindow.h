#pragma once
#include <QMainWindow>

class CMainWindow;

namespace Ui {
	class CStatsWindow;
};

class CStatsWindow : public QMainWindow
{
	Q_OBJECT

public:
	CStatsWindow(CMainWindow* wnd);

	void Update(bool breset = true);

private:
	void UpdateSelection(bool breset);

private slots:
	void on_actionSave_triggered();
	void on_actionClip_triggered();

private:
	CMainWindow*		m_wnd;
	Ui::CStatsWindow*	ui;
};
