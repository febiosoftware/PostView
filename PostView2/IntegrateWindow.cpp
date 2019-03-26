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
#include "GLDataMap.h"
#include "GLModel.h"
#include "GLPlaneCutPlot.h"

class Ui::CIntegrateWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			toolBar;
	QToolBar*			zoomBar;
	QComboBox*			dataSource;

	QAction*	actionZoomSelect;
	bool		updating;	// flag indicating we're inside Update

public:
	void setupUi(QMainWindow* parent)
	{
		updating = false;

		plot = new CPlotWidget(parent);
		plot->setObjectName("summaryPlot");
		plot->showLegend(false);
		parent->setCentralWidget(plot);

		toolBar = new QToolBar(parent);
		QAction* actionSave = toolBar->addAction(QIcon(QString(":/icons/save.png")), "Save"); actionSave->setObjectName("actionSave");
		QAction* actionClip = toolBar->addAction(QIcon(QString(":/icons/clipboard.png")), "Copy to clipboard"); actionClip->setObjectName("actionClip");
		toolBar->addWidget(new QLabel("Source: "));
		toolBar->addWidget(dataSource = new QComboBox);
		dataSource->setObjectName("dataSource");
		dataSource->setMinimumWidth(150);

		zoomBar = new QToolBar(parent);
		QAction* actionZoomWidth  = zoomBar->addAction(QIcon(QString(":/icons/zoom_width.png" )), "Zoom Width" ); actionZoomWidth->setObjectName("actionZoomWidth" );
		QAction* actionZoomHeight = zoomBar->addAction(QIcon(QString(":/icons/zoom_height.png")), "Zoom Height"); actionZoomHeight->setObjectName("actionZoomHeight");
		QAction* actionZoomFit    = zoomBar->addAction(QIcon(QString(":/icons/zoom_fit.png"   )), "Zoom Fit"   ); actionZoomFit->setObjectName("actionZoomFit"   );
		actionZoomSelect = zoomBar->addAction(QIcon(QString(":/icons/zoom_select.png")), "Zoom Select"); actionZoomSelect->setObjectName("actionZoomSelect"); actionZoomSelect->setCheckable(true);
		zoomBar->addSeparator();
		QAction* actionProps = zoomBar->addAction(QIcon(QString(":/icons/properties.png")), "Properties"); actionProps->setObjectName("actionProps");

		parent->addToolBar(Qt::TopToolBarArea, toolBar);
		parent->addToolBar(Qt::BottomToolBarArea, zoomBar);

		QMetaObject::connectSlotsByName(parent);
	}
};

CIntegrateWindow::CIntegrateWindow(CMainWindow* wnd) : m_wnd(wnd), QMainWindow(wnd), ui(new Ui::CIntegrateWindow)
{
	setWindowTitle("PostView2: Integrate");
	m_nsrc = -1;
	ui->setupUi(this);
	setMinimumWidth(500);
	resize(600, 500);
}

void CIntegrateWindow::Update(bool breset)
{
	CDocument* doc = m_wnd->GetActiveDocument();
	if (doc->IsValid() == false) return;

	// update the source options
	ui->updating = true;
	if (breset || (m_nsrc == -1)) UpdateSourceOptions();

	// Update integral
	UpdateIntegral();

	ui->updating = false;
	// redraw
	ui->plot->fitToData();
	ui->plot->repaint();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::UpdateIntegral()
{
	// clear the view
	ui->plot->clear();

	// get the source object
	CDocument* pdoc = m_wnd->GetActiveDocument();

	CGLModel* model = pdoc->GetGLModel();

	char sztitle[256] = {0};
	CLineChartData* data = new CLineChartData;
	CGLPlaneCutPlot* pp = m_src[m_nsrc];
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
	ui->plot->setTitle(sztitle);
	ui->plot->addPlotData(data);
	ui->plot->fitToData();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::UpdateSourceOptions()
{
	// clear the source options
	ui->dataSource->clear();
	m_src.clear();

	// Add the selection source
	ui->dataSource->addItem("current selection");
	m_src.push_back((CGLPlaneCutPlot*) 0);

	// get the document
	CDocument* pdoc = m_wnd->GetActiveDocument();

	// add all plane cuts to the source options
	GPlotList& plt = pdoc->GetPlotList();
	GPlotList::iterator it;
	for (it = plt.begin(); it != plt.end(); ++it)
	{
		CGLPlaneCutPlot* pp = dynamic_cast<CGLPlaneCutPlot*>(*it);
		if (pp) 
		{
			string name = pp->GetName();
			ui->dataSource->addItem(name.c_str());
			m_src.push_back(pp);
		}
	}

	if ((m_nsrc < 0) || (m_nsrc >= m_src.size()-1)) m_nsrc = 0;
	ui->dataSource->setCurrentIndex(m_nsrc);
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::IntegrateSelection(CLineChartData& data)
{
	// get the document
	CDocument* pdoc = m_wnd->GetActiveDocument();
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
	CDocument* pdoc = m_wnd->GetActiveDocument();
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

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Integrate Data", QDir::currentPath(), QString("All files (*)"));
	if (fileName.isEmpty() == false)
	{
		if (ui->plot->Save(fileName) == false)
			QMessageBox::critical(this, "Save Integrate Data", "A problem occurred saving the data.");
	}
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionClip_triggered()
{
	ui->plot->OnCopyToClipboard();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionProps_triggered()
{
	ui->plot->OnShowProps();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionZoomWidth_triggered()
{
	ui->plot->OnZoomToWidth();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionZoomHeight_triggered()
{
	ui->plot->OnZoomToHeight();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionZoomFit_triggered()
{
	ui->plot->OnZoomToFit();
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_actionZoomSelect_toggled(bool bchecked)
{
	ui->plot->ZoomToRect(bchecked);
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_summaryPlot_doneZoomToRect()
{
	ui->actionZoomSelect->setChecked(false);
}

//-----------------------------------------------------------------------------
void CIntegrateWindow::on_dataSource_currentIndexChanged(int index)
{
	if (ui->updating == false) 
	{
		m_nsrc = index;
		Update(false);
	}
}
