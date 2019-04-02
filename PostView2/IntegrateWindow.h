#pragma once
#include <QMainWindow>
#include <vector>
#include "Document.h"
#include "GraphWindow.h"

class CMainWindow;
class CGLPlaneCutPlot;
class FEMeshBase;
class FEState;
class FEModel;
class CLineChartData;

class CIntegrateWindow : public CGraphWindow
{
	Q_OBJECT

public:
	CIntegrateWindow(CMainWindow* wnd);

	void Update(bool breset = true, bool bfit = false) override;

private:
	double IntegrateNodes(FEMeshBase& mesh, FEState* ps);
	double IntegrateEdges(FEMeshBase& mesh, FEState* ps);
	double IntegrateFaces(FEMeshBase& mesh, FEState* ps);
	double IntegrateElems(FEMeshBase& mesh, FEState* ps);

	void UpdateSourceOptions();

	void UpdateIntegral();
	void IntegrateSelection(CLineChartData& data);
	void IntegratePlaneCut(CGLPlaneCutPlot* pp, CLineChartData& data);

private:
	std::vector<CGLPlaneCutPlot*>	m_src;
	int		m_nsrc;
	bool	m_updating;
};
