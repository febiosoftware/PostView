#pragma once
#include <QMainWindow>
#include <vector>

class CMainWindow;
class CGLPlaneCutPlot;
class FEMeshBase;
class FEState;

namespace Ui {
	class CIntegrateWindow;
};

class FEModel;
class CPlotData;

class CIntegrateWindow : public QMainWindow
{
	Q_OBJECT

public:
	CIntegrateWindow(CMainWindow* wnd);

	void Update(bool breset = true);

private:
	double IntegrateNodes(FEMeshBase& mesh, FEState* ps);
	double IntegrateEdges(FEMeshBase& mesh, FEState* ps);
	double IntegrateFaces(FEMeshBase& mesh, FEState* ps);
	double IntegrateElems(FEMeshBase& mesh, FEState* ps);

	void UpdateSourceOptions();

	void UpdateIntegral();
	void IntegrateSelection(CPlotData& data);
	void IntegratePlaneCut (CGLPlaneCutPlot* pp, CPlotData& data);

private slots:
	void on_actionSave_triggered();
	void on_actionClip_triggered();
	void on_actionProps_triggered();
	void on_actionZoomWidth_triggered();
	void on_actionZoomHeight_triggered();
	void on_actionZoomFit_triggered();
	void on_actionZoomSelect_toggled(bool bchecked);
	void on_summaryPlot_doneZoomToRect();
	void on_dataSource_currentIndexChanged(int);

private:
	CMainWindow*		m_wnd;
	Ui::CIntegrateWindow*	ui;
	std::vector<CGLPlaneCutPlot*>	m_src;
	int	m_nsrc;
};
