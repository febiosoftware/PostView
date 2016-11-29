#include "GraphWindow.h"
#include "PlotWidget.h"
#include "DataFieldSelector.h"
#include <QToolBar>
#include <qstackedwidget.h>
#include <QLabel>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <QToolBox>
#include <QLineEdit>
#include "GLDataMap.h"
#include "GLModel.h"
#include "version.h"
#include <PostViewLib/LinearRegression.h>

RegressionUi::RegressionUi(CGraphWidget* graph, QWidget* parent) : CPlotTool(parent), m_graph(graph)
{
	QVBoxLayout* l = new QVBoxLayout;
	l->addWidget(new QLabel("<p>y = <b>a</b>*x + <b>b</b></p>"));
	QHBoxLayout* h = new QHBoxLayout;
	QLabel* lbl = 0;
	h->addWidget(lbl = new QLabel("a")); h->addWidget(a = new QLineEdit); lbl->setBuddy(a);
	a->setValidator(new QDoubleValidator);
	a->setReadOnly(true);
	l->addLayout(h);
	h = new QHBoxLayout;
	h->addWidget(lbl = new QLabel("b")); h->addWidget(b = new QLineEdit); lbl->setBuddy(b);
	b->setValidator(new QDoubleValidator);
	b->setReadOnly(true);
	l->addLayout(h);

	QPushButton* b = new QPushButton("Calculate");
	l->addWidget(b);

	l->addStretch();
	setLayout(l);

	QObject::connect(b, SIGNAL(clicked()), this, SLOT(onCalculate()));

	Update();
}

void RegressionUi::onCalculate()
{
	Update();

	if (m_graph == 0) return;

	// get the first data field
	int plots = m_graph->plots();
	if (plots == 0) return;

	CPlotData& data = m_graph->getPlotData(0);
	int N = data.size();
	vector<pair<double, double> > pt(N);
	for (int i=0; i<N; ++i)
	{	
		QPointF p = data.Point(i);
		pt[i].first = p.x();
		pt[i].second = p.y();
	}
	pair<double, double> ans;
	if (LinearRegression(pt, ans))
	{
		m_a = ans.first;
		m_b = ans.second;
		a->setText(QString::number(m_a));
		b->setText(QString::number(m_b));
		m_bvalid = true;
		m_graph->repaint();
	}
}

void RegressionUi::draw(QPainter& p)
{
	if (m_bvalid==false) return;

	QRectF view = m_graph->m_viewRect;
	double x0 = view.left();
	double x1 = view.right();
	double y0 = m_a*x0 + m_b;
	double y1 = m_a*x1 + m_b;

	QPoint p0 = m_graph->ViewToScreen(QPointF(x0, y0));
	QPoint p1 = m_graph->ViewToScreen(QPointF(x1, y1));

	p.setPen(QPen(Qt::black, 2));
	p.drawLine(p0, p1);
}

void RegressionUi::Update()
{
	m_bvalid = false;
	a->clear();
	b->clear();
}

//=============================================================================
void CGraphWidget::paintEvent(QPaintEvent* pe)
{
	CPlotWidget::paintEvent(pe);

	QPainter p(this);
	p.setClipRect(m_screenRect);
	p.setRenderHint(QPainter::Antialiasing, true);
	for (size_t i = 0; i<m_tools.size(); ++i)
	{
		CPlotTool* tool = m_tools[i];
		tool->draw(p);
	}
}

void CGraphWidget::Update()
{
	for (size_t i = 0; i<m_tools.size(); ++i)
		m_tools[i]->Update();
	repaint();
}

//=============================================================================
class Ui::CGraphWindow
{
public:
	CGraphWidget*		plot;
	QToolBar*			toolBar;
	QToolBar*			zoomBar;
	QComboBox*			selectPlot;
	QStackedWidget*		selectXSource;
	QComboBox*			selectTime;
	CDataFieldSelector*	selectX;
	CDataFieldSelector*	selectY;
	QToolBox*			tools;

	QAction* actionSave;
	QAction* actionClipboard;
	QAction* actionProps;
	QAction* actionZoomSelect;

public:
	void setupUi(::CGraphWindow* parent)
	{
		QSplitter* centralWidget = new QSplitter;

		plot = new CGraphWidget(parent);
		plot->setObjectName("plot");

		centralWidget->addWidget(plot);
		centralWidget->addWidget(tools = new QToolBox); tools->hide();
		parent->setCentralWidget(centralWidget);

		CPlotTool* tool = new RegressionUi(plot);
		tools->addItem(tool, "Linear Regression");
		plot->addTool(tool);

		toolBar = new QToolBar(parent);
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
		actionSave = toolBar->addAction(QIcon(QString(":/icons/save.png")), "Save"); actionSave->setObjectName("actionSave");
		actionClipboard = toolBar->addAction(QIcon(QString(":/icons/clipboard.png")), "Copy to clipboard"); actionClipboard->setObjectName("actionClipboard");
		toolBar->addWidget(new QLabel("Type: "));
		toolBar->addWidget(selectPlot);
		toolBar->addWidget(new QLabel(" X: "));
		toolBar->addWidget(selectXSource);
		toolBar->addWidget(new QLabel(" Y: "));
		toolBar->addWidget(selectY);
		QPushButton* showTools = new QPushButton("Tools");
		showTools->setCheckable(true);
		showTools->setChecked(false);
		toolBar->addWidget(showTools);

		zoomBar = new QToolBar(parent);
		QAction* actionZoomWidth  = zoomBar->addAction(QIcon(QString(":/icons/zoom_width.png" )), "Zoom Width" ); actionZoomWidth->setObjectName("actionZoomWidth" );
		QAction* actionZoomHeight = zoomBar->addAction(QIcon(QString(":/icons/zoom_height.png")), "Zoom Height"); actionZoomHeight->setObjectName("actionZoomHeight");
		QAction* actionZoomFit    = zoomBar->addAction(QIcon(QString(":/icons/zoom_fit.png"   )), "Zoom Fit"   ); actionZoomFit->setObjectName("actionZoomFit"   );
		actionZoomSelect = zoomBar->addAction(QIcon(QString(":/icons/zoom_select.png")), "Zoom Select"); actionZoomSelect->setObjectName("actionZoomSelect"); actionZoomSelect->setCheckable(true);
		zoomBar->addSeparator();
		actionProps = zoomBar->addAction(QIcon(QString(":/icons/properties.png")), "Properties"); actionProps->setObjectName("actionProps");

		parent->addToolBar(Qt::TopToolBarArea, toolBar);
		parent->addToolBar(Qt::BottomToolBarArea, zoomBar);

		QObject::connect(showTools, SIGNAL(clicked(bool)), tools, SLOT(setVisible(bool)));

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

	FEMeshBase& mesh = *doc->GetFEModel()->GetFEMesh(0);

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
	ui->plot->Update();
}

//-----------------------------------------------------------------------------
void CGraphWindow::addSelectedNodes()
{
	CDocument* pdoc = m_wnd->GetDocument();
	FEModel& fem = *pdoc->GetFEModel();
	FEMeshBase& mesh = *fem.GetFEMesh(0);

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
	FEMeshBase& mesh = *fem.GetFEMesh(0);

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
	FEMeshBase& mesh = *fem.GetFEMesh(0);

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
	FEMeshBase& mesh = *fem.GetFEMesh(0);

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
