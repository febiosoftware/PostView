#include "GraphWindow.h"
#include "PlotWidget.h"
#include "DataFieldSelector.h"
#include <QToolbar>
#include <QStackedWidget.h>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include "GLDataMap.h"
#include "GLModel.h"

class Ui::CGraphWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			tool;
	QComboBox*			selectPlot;
	QStackedWidget*		selectXSource;
	QComboBox*			selectTime;
	CDataFieldSelector*	selectX;
	CDataFieldSelector*	selectY;

public:
	void setupUi(::CGraphWindow* parent)
	{
		plot = new CPlotWidget(parent);
		parent->setCentralWidget(plot);

		tool = new QToolBar(parent);
		{
			selectPlot = new QComboBox;
			selectPlot->setObjectName("selectPlot");
			selectPlot->addItem("Line");
			selectPlot->addItem("Scatter");

			selectXSource = new QStackedWidget;
			{
				selectTime = new QComboBox;
				selectTime->setObjectName("selectTime");
				selectTime->addItem("Time");
				selectTime->addItem("Steps");

				selectX = new CDataFieldSelector;
				selectX->setObjectName("selectX");
			}
			selectXSource->addWidget(selectTime);
			selectXSource->addWidget(selectX);
			selectXSource->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

			selectY = new CDataFieldSelector;
			selectY->setObjectName("selectY");
		}
		tool->addWidget(selectPlot);
		tool->addWidget(selectXSource);
		tool->addWidget(selectY);

		parent->addToolBar(Qt::TopToolBarArea, tool);

		QMetaObject::connectSlotsByName(parent);
	}
};

CGraphWindow::CGraphWindow(CMainWindow* pwnd) : m_wnd(pwnd), QMainWindow(pwnd), ui(new Ui::CGraphWindow)
{
	m_bUserRange = true;
	m_bAutoRange = false;
	m_bTrackTime = false;
	m_nUserMin = 0;
	m_nUserMax = -1;

	m_nTimeMin = -1;
	m_nTimeMax = -1;

	ui->setupUi(this);
	setMinimumWidth(500);
	resize(500, 400);
}

//-----------------------------------------------------------------------------
// If breset==true, a new model was loaded. 
// If breset==false, the selection has changed
void CGraphWindow::Update(bool breset)
{
	CDocument* doc = m_wnd->GetDocument();
	if (breset)
	{
		if (doc->IsValid())
		{
			ui->selectX->BuildMenu(doc->GetFEModel(), DATA_FLOAT);
			ui->selectY->BuildMenu(doc->GetFEModel(), DATA_FLOAT);
		}
	}

	// Currently, when the time step changes, Update is called with breset set to false.
	// Depending on the time range settings, we may or may not need to update the track view.

	// when the user sets the range, we don't have to do anything so let's return
//	if (m_bUserRange && (breset == false)) return;

	// get the document and current time point and time steps
	CDocument* pdoc = m_wnd->GetDocument();
	int ntime  = pdoc->currentTime();
	int nsteps = pdoc->GetTimeSteps();

	// Figure out the time range
	int nmin = 0, nmax = 0;
	if (m_bUserRange)
	{
		// get the user defined range
		nmin = m_nUserMin;
		nmax = m_nUserMax;
	}
	else if (m_bAutoRange)
	{
//		m_wnd->GetTimeController()->GetRange(nmin, nmax);
	}
	else if (m_bTrackTime)
	{
		// simply set the min and max to the same value
		nmin = nmax = ntime;
	}

	// validate range
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;

	// When a reset is not required, see if the range has actually changed
	if (breset == false)
	{
		if ((nmin == m_nTimeMin) && (nmax == m_nTimeMax)) return;
	}

	// get the graph of the track view and clear it
	ui->plot->clear();

	// plot type
	int ntype = ui->selectPlot->currentIndex();

	// get the field data
	int dataX = ui->selectX->currentValue();
	int dataY = ui->selectY->currentValue();
	if ((ntype==1) && (dataX<=0)) return;
	if (dataY<=0) return;

	// set current time point index (TODO: Not sure if this is still used)
//	pview->SetCurrentTimeIndex(ntime);

	CGLModel* po = pdoc->GetGLModel();
	FEModel* pfem = pdoc->GetFEModel();

	FEMesh* pfe = pdoc->GetFEModel()->GetMesh();

	// get the title
	if (ntype == 0)
	{
		ui->plot->setTitle(ui->selectY->currentText());
	}
	else
	{
		QString xtext = ui->selectX->currentText();
		QString ytext = ui->selectY->currentText();

		ui->plot->setTitle(QString("%1 --- %2").arg(xtext).arg(ytext));
	}

	int nelems = pfe->Elements();
	int nodes  = pfe->Nodes();
	int nfaces = pfe->Faces();

	char str[256];

	// allocate temporary buffers for storing plot data
	int cx = nmax - nmin + 1;
	float* px = new float[cx];
	float* py = new float[cx];

	// we need to update the displacement map for all time steps
	// since the strain calculations depend on it
	CGLDisplacementMap* pdm = po->GetDisplacementMap();
	if (pdm)
	{
		for (int i=0; i<nsteps; ++i) po->GetDisplacementMap()->UpdateState(i);
	}

	int ncx = ui->selectTime->currentIndex();

	// get the selected elements
	for (int i=0; i<nelems; i++)
	{
		FEElement& e = pfe->Element(i);
		if (e.IsSelected())
		{
			// evaluate x-field
			if (ntype == 0)
			{
				if (ncx == 0) 
					for (int j=0; j<cx; j++) px[j] = pfem->GetState(j + nmin)->m_time;
				else
					for (int j=0; j<cx; j++) px[j] = (float) j+1.f + nmin;
			}
			else
			{
				TrackElementHistory(i, px, dataX, nmin, nmax);
			}

			// evaluate y-field
			TrackElementHistory(i, py, dataY, nmin, nmax);

			sprintf(str, "E%d", i+1);
			CPlotData plot;
			for (int j=0; j<cx; ++j) plot.addPoint(px[j], py[j]);
			ui->plot->addPlotData(plot);

//			pview->AddSeries(px, py, cx, str, i);
			
		}
	}

	// get the selected faces
	for (int i=0; i<nfaces; ++i)
	{
		FEFace& f = pfe->Face(i);
		if (f.IsSelected())
		{
			// evaluate x-field
			if (ntype == 0)
			{
				if (ncx == 0)
					for (int j=0; j<cx; j++) px[j] = pfem->GetState(j + nmin)->m_time;
				else
					for (int j=0; j<cx; j++) px[j] = (float)j + 1.f + nmin;
			}
			else
				TrackFaceHistory(i, px, dataX, nmin, nmax);

			// evaluate y-field
			TrackFaceHistory(i, py, dataY, nmin, nmax);

			sprintf(str, "F%d", i+1);
//			pview->AddSeries(px, py, cx, str, i);			
			CPlotData plot;
			for (int j=0; j<cx; ++j) plot.addPoint(px[j], py[j]);
			ui->plot->addPlotData(plot);
		}
	}

	// get the selected nodes
	for (int i=0; i<nodes; i++)
	{
		FENode& n = pfe->Node(i);
		if (n.IsSelected())
		{
			// evaluate x-field
			if (ntype == 0)
			{
				if (ncx == 0)
					for (int j=0; j<cx; j++) px[j] = pfem->GetState(j + nmin)->m_time;
				else 
					for (int j=0; j<cx; j++) px[j] = (float) j +1.f + nmin;
			}
			else
				TrackNodeHistory(i, px, dataX, nmin, nmax);

			// evaluate y-field
			TrackNodeHistory(i, py, dataY, nmin, nmax);

			sprintf(str, "N%d", i+1);
//			pview->AddSeries(px, py, cx, str, -i-1);
			CPlotData plot;
			for (int j=0; j<cx; ++j) plot.addPoint(px[j], py[j]);
			ui->plot->addPlotData(plot);
		}
	}

	// clean up
	delete [] px;
	delete [] py;

	// redraw
	ui->plot->fitToData();
	ui->plot->repaint();
}

//-----------------------------------------------------------------------------
// Calculate time history of a node
void CGraphWindow::TrackNodeHistory(int node, float* pval, int nfield, int nmin, int nmax)
{
	FEModel& fem = *m_wnd->GetDocument()->GetFEModel();

	int nsteps = fem.GetStates();
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;
	int nn = nmax - nmin + 1;

	NODEDATA nd;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateNode(node, n + nmin, nfield, nd);
		pval[n] = nd.m_val;
	}
}

//-----------------------------------------------------------------------------
// Calculate time history of a face
void CGraphWindow::TrackFaceHistory(int nface, float* pval, int nfield, int nmin, int nmax)
{
	FEModel& fem = *m_wnd->GetDocument()->GetFEModel();

	int nsteps = fem.GetStates();
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;
	int nn = nmax - nmin + 1;

	FACEDATA f;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateFace(nface, n + nmin, nfield, f);
		pval[n] = f.m_val;
	}
}

//-----------------------------------------------------------------------------
// Calculate time history of an element
void CGraphWindow::TrackElementHistory(int nelem, float* pval, int nfield, int nmin, int nmax)
{
	FEModel& fem = *m_wnd->GetDocument()->GetFEModel();

	int nsteps = fem.GetStates();
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;
	int nn = nmax - nmin + 1;

	ELEMDATA e;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateElement(nelem, n + nmin, nfield, e);
		pval[n] = e.m_val;
	}
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectTime_currentIndexChanged(int)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectX_currentIndexChanged(int)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectY_currentIndexChanged(int)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectPlot_currentIndexChanged(int index)
{
	ui->selectXSource->setCurrentIndex(index);
	Update(false);
}
