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
#include <PostViewLib/constants.h>
#include <PostGL/GLDataMap.h>
#include <PostGL/GLModel.h>
#include <PostGL/GLPlaneCutPlot.h>
using namespace Post;

CIntegrateWindow::CIntegrateWindow(CMainWindow* wnd) : CGraphWindow(wnd)
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
	GPlotList& plt = pdoc->GetPlotList();
	GPlotList::iterator it;
	for (it = plt.begin(); it != plt.end(); ++it)
	{
		CGLPlaneCutPlot* pp = dynamic_cast<CGLPlaneCutPlot*>(*it);
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
	FEMeshBase& mesh = *fem.GetFEMesh(0);
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
double CIntegrateWindow::IntegrateNodes(FEMeshBase& mesh, FEState* ps)
{
	double res = 0.0;
	int N = mesh.Nodes();
	for (int i=0; i<N; ++i)
	{
		FENode& node = mesh.Node(i);
		if (node.IsSelected() && (ps->m_NODE[i].m_ntag > 0))
		{
			res += ps->m_NODE[i].m_val;
		}
	}
	return res;
}

//-----------------------------------------------------------------------------
double CIntegrateWindow::IntegrateEdges(FEMeshBase& mesh, FEState* ps)
{
	assert(false);
	return 0.0;
}

//-----------------------------------------------------------------------------
// This function calculates the integral over a surface. Note that if the surface
// is triangular, then we calculate the integral from a degenerate quad.
double CIntegrateWindow::IntegrateFaces(FEMeshBase& mesh, FEState* ps)
{
	double res = 0.0;
	float v[4];
	vec3f r[4];
	for (int i=0; i<mesh.Faces(); ++i)
	{
		FEFace& f = mesh.Face(i);
		if (f.IsSelected() && f.IsActive())
		{
			int nn = f.Nodes();

			// get the nodal values
			for (int j=0; j<nn; ++j) v[j] = ps->m_NODE[f.node[j]].m_val;
			if (nn==3) v[3] = v[2];

			// get the nodal coordinates
			for (int j=0; j<nn; ++j) r[j] = ps->m_NODE[f.node[j]].m_rt;
			if (nn==3) r[3] = r[2];

			// add to integral
			res += mesh.IntegrateQuad(r, v);
		}
	}
	return res;
}

//-----------------------------------------------------------------------------
// This function calculates the integral over a volume. Note that if the volume
// is not hexahedral, then we calculate the integral from a degenerate hex.
double CIntegrateWindow::IntegrateElems(FEMeshBase& mesh, FEState* ps)
{
	double res = 0.0;
	float v[8];
	vec3f r[8];
	for (int i=0; i<mesh.Elements(); ++i)
	{
		FEElement& e = mesh.Element(i);
		if (e.IsSelected() && (e.IsSolid()) && (ps->m_ELEM[i].m_state & StatusFlags::ACTIVE))
		{
			int nn = e.Nodes();

			// get the nodal values and coordinates
			for (int j=0; j<nn; ++j) v[j] = ps->m_NODE[e.m_node[j]].m_val;
			for (int j=0; j<nn; ++j) r[j] = ps->m_NODE[e.m_node[j]].m_rt;
			switch (e.Type())
			{
			case FE_PENTA6:
				v[7] = v[5]; r[7] = r[5];
				v[6] = v[5]; r[6] = r[5];
				v[5] = v[4]; r[5] = r[4];
				v[4] = v[3]; r[4] = r[3];
				v[3] = v[2]; r[3] = r[2];
				v[2] = v[2]; r[2] = r[2];
				v[1] = v[1]; r[1] = r[1];
				v[0] = v[0]; r[0] = r[0];
				break;
			case FE_TET4:
			case FE_TET5:
				v[7] = v[3]; r[7] = r[3];
				v[6] = v[3]; r[6] = r[3];
				v[5] = v[3]; r[5] = r[3];
				v[4] = v[3]; r[4] = r[3];
				v[3] = v[2]; r[3] = r[2];
				v[2] = v[2]; r[2] = r[2];
				v[1] = v[1]; r[1] = r[1];
				v[0] = v[0]; r[0] = r[0];
				break;
			}
			
			// add to integral
			res += mesh.IntegrateHex(r, v);
		}
	}
	return res;
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
