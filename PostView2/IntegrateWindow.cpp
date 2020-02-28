#include "stdafx.h"
#include "IntegrateWindow.h"
#include "MainWindow.h"
#include "Document.h"
#include "PlotWidget.h"
#include "DataFieldSelector.h"
#include <QComboBox>
#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <PostLib/constants.h>
#include <PostGL/GLDataMap.h>
#include <PostGL/GLModel.h>
#include <PostGL/GLPlaneCutPlot.h>
using namespace Post;

CIntegrateWindow::CIntegrateWindow(CMainWindow* wnd) : CGraphWindow(wnd, 0)
{
	CDocument* doc = GetDocument();
	QString title = "PostView2: Integrate";
	if (doc) title += " - " + QString::fromStdString(doc->GetFileName());
	setWindowTitle(title);
	m_nsrc = -1;
}

void CIntegrateWindow::Update(bool breset, bool bfit)
{
	CDocument* doc = GetDocument();
	if (doc->IsValid() == false) return;

	// update the source options
	m_updating = true;
	if (breset || (m_nsrc == -1)) UpdateSourceOptions();

	// Update integral
	UpdateIntegral();

	m_updating = false;

	// redraw
	FitPlotsToData();
	RedrawPlot();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::UpdateIntegral()
{
	// clear the view
	ClearPlots();

	// get the source object
	CDocument* pdoc = GetDocument();

	CGLModel* model = pdoc->GetGLModel();

	int nsrc = GetCurrentYValue();
	if (nsrc < 0) return;

	char sztitle[256] = {0};
	CLineChartData* data = new CLineChartData;
	CGLPlaneCutPlot* pp = m_src[nsrc];
	if (pp == 0) 
	{
		// update based on current selection
		IntegrateSelection(*data);

		int nview = model->GetSelectionMode();
		sprintf(sztitle, "%s of %s", (nview == SELECT_NODES? "Sum" : "Integral"), pdoc->GetFieldString().c_str());
	}
	else 
	{
		// update based on plane cut plot
		IntegratePlaneCut(pp, *data);
		sprintf(sztitle, "%s of %s", "Integral", pdoc->GetFieldString().c_str());
	}
	data->setLabel("Value");
	SetPlotTitle(sztitle);
	AddPlotData(data);
	FitPlotsToData();

	UpdatePlots();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::UpdateSourceOptions()
{
	// Add the selection source
	CGenericDataSelector* sel = new CGenericDataSelector();
	sel->AddOption("current selection");
	m_src.push_back((CGLPlaneCutPlot*) 0);

	// get the document
	CDocument* pdoc = GetDocument();

	// add all plane cuts to the source options
	GPlotList& plt = pdoc->GetGLModel()->GetPlotList();
	for (int i=0; i<plt.Size(); ++i)
	{
		CGLPlaneCutPlot* pp = dynamic_cast<CGLPlaneCutPlot*>(plt[i]);
		if (pp) 
		{
			string name = pp->GetName();
			sel->AddOption(QString::fromStdString(name));
			m_src.push_back(pp);
		}
	}

	if ((m_nsrc < 0) || (m_nsrc >= m_src.size()-1)) m_nsrc = 0;
	SetYDataSelector(sel, m_nsrc);
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::IntegrateSelection(CLineChartData& data)
{
	// get the document
	CDocument* pdoc = GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	Post::FEPostMesh& mesh = *fem.GetFEMesh(0);
	CGLModel* po = pdoc->GetGLModel();

	data.clear();

	// get view mode
	int nview = po->GetSelectionMode();

	// make sure the color map is active
	if (po->GetColorMap()->IsActive())
	{
		// get the number of time steps
		int ntime = pdoc->GetTimeSteps();

		// make sure all states are up-to-date
		pdoc->UpdateAllStates();

		// loop over all steps
		for (int i=0; i<ntime; ++i)
		{
			FEState* ps = fem.GetState(i);

			// evaluate sum/integration
			double res = 0.0;
			if      (nview == SELECT_NODES) res = IntegrateNodes(mesh, ps);
			else if (nview == SELECT_EDGES) res = IntegrateEdges(mesh, ps);
			else if (nview == SELECT_FACES) res = IntegrateFaces(mesh, ps);
			else if (nview == SELECT_ELEMS) res = IntegrateElems(mesh, ps);
			else assert(false);

			data.addPoint(ps->m_time, res);
		}
	}
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::IntegratePlaneCut(CGLPlaneCutPlot* pp, CLineChartData& data)
{
	// get the document
	CDocument* pdoc = GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	CGLModel* po = pdoc->GetGLModel();

	data.clear();

	// make sure the color map is active
	if (po->GetColorMap()->IsActive())
	{
		// get the number of time steps
		int ntime = pdoc->GetTimeSteps();

		// make sure all states are up-to-date
		pdoc->UpdateAllStates();

		// loop over all steps
		for (int i=0; i<ntime; ++i)
		{
			FEState* ps = fem.GetState(i);
			data.addPoint(ps->m_time, pp->Integrate(ps));
		}
	}
}
