#pragma once
#include <QMainWindow>

class CMainWindow;

namespace Ui {
	class CSummaryWindow;
};

class FEModel;

class CSummaryWindow : public QMainWindow
{
	Q_OBJECT

public:
	struct RANGE
	{
		float	fmax, fmin, favg;
	};

public:
	CSummaryWindow(CMainWindow* wnd);

	void Update(bool breset = true);

private slots:
	void on_selectData_currentValueChanged(int);
	void on_actionSave_triggered();
	void on_actionClip_triggered();
	void on_actionProps_triggered();
	void on_actionZoomWidth_triggered();
	void on_actionZoomHeight_triggered();
	void on_actionZoomFit_triggered();
	void on_actionZoomSelect_toggled(bool bchecked);
	void on_summaryPlot_doneZoomToRect();
	void on_selectionOnly_clicked();
	void on_volumeAverage_clicked();

private:
	RANGE EvalNodeRange(FEModel& fem, int ntime, bool bsel);
	RANGE EvalEdgeRange(FEModel& fem, int ntime, bool bsel);
	RANGE EvalFaceRange(FEModel& fem, int ntime, bool bsel, bool bvol);
	RANGE EvalElemRange(FEModel& fem, int ntime, bool bsel, bool bvol);

private:
	CMainWindow*		m_wnd;
	Ui::CSummaryWindow*	ui;
	int		m_ncurrentData;
};
