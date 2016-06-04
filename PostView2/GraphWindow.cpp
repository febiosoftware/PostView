#include "GraphWindow.h"
#include "PlotWidget.h"
#include "DataFieldSelector.h"
#include <QToolbar>
#include <QStackedWidget.h>
#include <QLabel>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include "GLDataMap.h"
#include "GLModel.h"
#include "version.h"

class Ui::CGraphWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			tool;
	QToolBar*			zoomBar;
	QComboBox*			selectPlot;
	QStackedWidget*		selectXSource;
	QComboBox*			selectTime;
	CDataFieldSelector*	selectX;
	CDataFieldSelector*	selectY;

	QAction* actionSave;
	QAction* actionClipboard;
	QAction* actionProps;
	QAction* actionZoomSelect;

public:
	void setupUi(::CGraphWindow* parent)
	{
		plot = new CPlotWidget(parent);
		plot->setObjectName("plot");
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
				selectX->setMinimumWidth(150);
			}
			selectXSource->addWidget(selectTime);
			selectXSource->addWidget(selectX);
			selectXSource->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

			selectY = new CDataFieldSelector;
			selectY->setObjectName("selectY");
			selectY->setMinimumWidth(150);
		}
		actionSave = tool->addAction(QIcon(QString(":/icons/save.png")), "Save"); actionSave->setObjectName("actionSave");
		actionClipboard = tool->addAction(QIcon(QString(":/icons/clipboard.png")), "Copy to clipboard"); actionClipboard->setObjectName("actionClipboard");
		tool->addWidget(new QLabel("Type: "));
		tool->addWidget(selectPlot);
		tool->addWidget(new QLabel(" X: "));
		tool->addWidget(selectXSource);
		tool->addWidget(new QLabel(" Y: "));
		tool->addWidget(selectY);

		zoomBar = new QToolBar(parent);
		QAction* actionZoomWidth  = zoomBar->addAction(QIcon(QString(":/icons/zoom_width.png" )), "Zoom Width" ); actionZoomWidth->setObjectName("actionZoomWidth" );
		QAction* actionZoomHeight = zoomBar->addAction(QIcon(QString(":/icons/zoom_height.png")), "Zoom Height"); actionZoomHeight->setObjectName("actionZoomHeight");
		QAction* actionZoomFit    = zoomBar->addAction(QIcon(QString(":/icons/zoom_fit.png"   )), "Zoom Fit"   ); actionZoomFit->setObjectName("actionZoomFit"   );
		actionZoomSelect = zoomBar->addAction(QIcon(QString(":/icons/zoom_select.png")), "Zoom Select"); actionZoomSelect->setObjectName("actionZoomSelect"); actionZoomSelect->setCheckable(true);
		zoomBar->addSeparator();
		actionProps = zoomBar->addAction(QIcon(QString(":/icons/properties.png")), "Properties"); actionProps->setObjectName("actionProps");

		parent->addToolBar(Qt::TopToolBarArea, tool);
		parent->addToolBar(Qt::BottomToolBarArea, zoomBar);

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
	resize(600, 500);
}

//-----------------------------------------------------------------------------
// If breset==true, a new model was loaded. 
// If breset==false, the selection has changed
void CGraphWindow::Update(bool breset)
{
	CDocument* doc = m_wnd->GetDocument();
	if (doc->IsValid() == false) return;

	if (breset)
	{
		ui->selectX->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
		ui->selectY->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
	}

	// Currently, when the time step changes, Update is called with breset set to false.
	// Depending on the time range settings, we may or may not need to update the track view.

	// when the user sets the range, we don't have to do anything so let's return
//	if (m_bUserRange && (breset == false)) return;

	// get the document and current time point and time steps
	int ntime  = doc->currentTime();
	int nsteps = doc->GetTimeSteps();

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
	int ncx = ui->selectTime->currentIndex();
	if (ntype == 0) m_xtype = ncx; else m_xtype = 2;

	// get the field data
	m_dataX = ui->selectX->currentValue();
	m_dataY = ui->selectY->currentValue();
	if ((ntype==1) && (m_dataX<=0)) return;
	if (m_dataY<=0) return;

	// set current time point index (TODO: Not sure if this is still used)
//	pview->SetCurrentTimeIndex(ntime);

	CGLModel* po = doc->GetGLModel();
	FEModel& fem = *doc->GetFEModel();

	FEMeshBase& mesh = *doc->GetFEModel()->GetMesh();

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

	m_firstState = nmin;
	m_lastState  = nmax;

	// we need to update the displacement map for all time steps
	// since the strain calculations depend on it
	CGLDisplacementMap* pdm = po->GetDisplacementMap();
	if (pdm)
	{
		for (int i=0; i<nsteps; ++i) po->GetDisplacementMap()->UpdateState(i);
	}

	// add selections
	addSelectedNodes();
	addSelectedEdges();
	addSelectedFaces();
	addSelectedElems();

	// redraw
	ui->plot->fitToData();
	ui->plot->repaint();
}

//-----------------------------------------------------------------------------
void CGraphWindow::addSelectedNodes()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *fem.GetMesh();

	int nsteps = m_lastState - m_firstState + 1;
	vector<float> xdata(nsteps);
	vector<float> ydata(nsteps);

	// get the selected nodes
	int NN = mesh.Nodes();
	for (int i=0; i<NN; i++)
	{
		FENode& node = mesh.Node(i);
		if (node.IsSelected())
		{
			// evaluate x-field
			switch (m_xtype)
			{
			case 0: 
				for (int j=0; j<nsteps; j++) xdata[j] = fem.GetState(j + m_firstState)->m_time;
				break;
			case 1:
				for (int j=0; j<nsteps; j++) xdata[j] = (float) j + 1.f + m_firstState;
				break;
			default:
				TrackNodeHistory(i, &xdata[0], m_dataX, m_firstState, m_lastState);
			}

			// evaluate y-field
			TrackNodeHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

			CPlotData plot;
			plot.setLabel(QString("N%1").arg(i+1));
			for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
			ui->plot->addPlotData(plot);
		}
	}
}

//-----------------------------------------------------------------------------
void CGraphWindow::addSelectedEdges()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *fem.GetMesh();

	int nsteps = m_lastState - m_firstState + 1;
	vector<float> xdata(nsteps);
	vector<float> ydata(nsteps);

	// get the selected nodes
	int NL = mesh.Edges();
	for (int i=0; i<NL; i++)
	{
		FEEdge& edge = mesh.Edge(i);
		if (edge.IsSelected())
		{
			// evaluate x-field
			switch (m_xtype)
			{
			case 0: 
				for (int j=0; j<nsteps; j++) xdata[j] = fem.GetState(j + m_firstState)->m_time;
				break;
			case 1:
				for (int j=0; j<nsteps; j++) xdata[j] = (float) j + 1.f + m_firstState;
				break;
			default:
				TrackEdgeHistory(i, &xdata[0], m_dataX, m_firstState, m_lastState);
			}

			// evaluate y-field
			TrackEdgeHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

			CPlotData plot;
			plot.setLabel(QString("L%1").arg(i+1));
			for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
			ui->plot->addPlotData(plot);
		}
	}
}

//-----------------------------------------------------------------------------
void CGraphWindow::addSelectedFaces()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *fem.GetMesh();

	int nsteps = m_lastState - m_firstState + 1;
	vector<float> xdata(nsteps);
	vector<float> ydata(nsteps);

	// get the selected faces
	int NF = mesh.Faces();
	for (int i=0; i<NF; ++i)
	{
		FEFace& f = mesh.Face(i);
		if (f.IsSelected())
		{
			// evaluate x-field
			switch (m_xtype)
			{
			case 0: 
				for (int j=0; j<nsteps; j++) xdata[j] = fem.GetState(j + m_firstState)->m_time;
				break;
			case 1:
				for (int j=0; j<nsteps; j++) xdata[j] = (float) j + 1.f + m_firstState;
				break;
			default:
				TrackFaceHistory(i, &xdata[0], m_dataX, m_firstState, m_lastState);
			}

			// evaluate y-field
			TrackFaceHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

			CPlotData plot;
			plot.setLabel(QString("F%1").arg(i+1));
			for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
			ui->plot->addPlotData(plot);
		}
	}
}

//-----------------------------------------------------------------------------
void CGraphWindow::addSelectedElems()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *fem.GetMesh();

	int nsteps = m_lastState - m_firstState + 1;
	vector<float> xdata(nsteps);
	vector<float> ydata(nsteps);

	// get the selected elements
	int NE = mesh.Elements();
	for (int i=0; i<NE; i++)
	{
		FEElement& e = mesh.Element(i);
		if (e.IsSelected())
		{
			// evaluate x-field
			switch (m_xtype)
			{
			case 0: 
				for (int j=0; j<nsteps; j++) xdata[j] = fem.GetState(j + m_firstState)->m_time;
				break;
			case 1:
				for (int j=0; j<nsteps; j++) xdata[j] = (float) j + 1.f + m_firstState;
				break;
			default:
				TrackElementHistory(i, &xdata[0], m_dataX, m_firstState, m_lastState);
			}

			// evaluate y-field
			TrackElementHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

			CPlotData plot;
			for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
			plot.setLabel(QString("E%1").arg(i+1));
			ui->plot->addPlotData(plot);
		}
	}
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
// Calculate time history of a edge
void CGraphWindow::TrackEdgeHistory(int edge, float* pval, int nfield, int nmin, int nmax)
{
	FEModel& fem = *m_wnd->GetDocument()->GetFEModel();

	int nsteps = fem.GetStates();
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;
	int nn = nmax - nmin + 1;

	EDGEDATA nd;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateEdge(edge, n + nmin, nfield, nd);
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

	float data[FEFace::MAX_NODES], val;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateFace(nface, n + nmin, nfield, data, val);
		pval[n] = val;
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

	float data[FEGenericElement::MAX_NODES] = {0.f}, val;
	for (int n=0; n<nn; n++)
	{
		fem.EvaluateElement(nelem, n + nmin, nfield, data, val);
		pval[n] = val;
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

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Graph Data", QDir::currentPath(), QString("All files (*)"));
	if (fileName.isEmpty() == false)
	{
		if (ui->plot->Save(fileName) == false)
			QMessageBox::critical(this, "Save Graph Data", "A problem occurred saving the data.");
	}
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionClipboard_triggered()
{
	ui->plot->OnCopyToClipboard();
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionProps_triggered()
{
	ui->plot->OnShowProps();
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionZoomWidth_triggered()
{
	ui->plot->OnZoomToWidth();
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionZoomHeight_triggered()
{
	ui->plot->OnZoomToHeight();
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionZoomFit_triggered()
{
	ui->plot->OnZoomToFit();
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionZoomSelect_toggled(bool bchecked)
{
	ui->plot->ZoomToRect(bchecked);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_plot_doneZoomToRect()
{
	ui->actionZoomSelect->setChecked(false);
}
