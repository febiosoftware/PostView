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
#include <QCheckBox>
#include <QRadioButton>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>
#include <QToolBox>
#include <QLineEdit>
#include "GLDataMap.h"
#include "GLModel.h"
#include "version.h"
#include <QValidator>
#include <QComboBox>
#include <PostViewLib/LinearRegression.h>

OptionsUi::OptionsUi(CGraphWidget* graph, QWidget* parent) : CPlotTool(parent)
{
	QVBoxLayout* l = new QVBoxLayout;
	l->addWidget(a[0] = new QRadioButton("Time step range"));
	l->addWidget(a[1] = new QRadioButton("Current time step"));
	l->addWidget(a[2] = new QRadioButton("User range:"));
	l->addWidget(range = new QLineEdit);
	l->addWidget(smoothLines = new QCheckBox("Smooth lines"));
	l->addWidget(dataMarks   = new QCheckBox("Show data marks"));
	l->addStretch();
	setLayout(l);

#ifdef __APPLE__
	smoothLines->setChecked(false);
	dataMarks->setChecked(false);
#else
	smoothLines->setChecked(true);
	dataMarks->setChecked(true);
#endif

	a[0]->setChecked(true);

	QObject::connect(a[0], SIGNAL(clicked()), this, SLOT(onOptionsChanged()));
	QObject::connect(a[1], SIGNAL(clicked()), this, SLOT(onOptionsChanged()));
	QObject::connect(a[2], SIGNAL(clicked()), this, SLOT(onOptionsChanged()));
	QObject::connect(range, SIGNAL(editingFinished()), this, SLOT(onOptionsChanged()));
	QObject::connect(smoothLines, SIGNAL(stateChanged(int)), SLOT(onOptionsChanged()));
	QObject::connect(dataMarks  , SIGNAL(stateChanged(int)), SLOT(onOptionsChanged()));
}

void OptionsUi::onOptionsChanged()
{
	emit optionsChanged();
}

int OptionsUi::currentOption()
{
	if (a[0]->isChecked()) return 0;
	if (a[1]->isChecked()) return 1;
	if (a[2]->isChecked()) return 2;
	return -1;
}

bool OptionsUi::lineSmoothing()
{
	return smoothLines->isChecked();
}

bool OptionsUi::showDataMarks()
{
	return dataMarks->isChecked();
}

void OptionsUi::setUserRange(int imin, int imax)
{
	range->setText(QString("%1:%2").arg(imin).arg(imax));
}

void OptionsUi::getUserRange(int& imin, int& imax)
{
	QStringList l = range->text().split(':');
	imin = imax = 0;
	if (l.size() == 1)
	{
		imin = imax = l.at(0).toInt();
	}
	else if (l.size() > 1)
	{
		imin = l.at(0).toInt();
		imax = l.at(1).toInt();
	}
}

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

	OptionsUi*	ops;

public:
	void setupUi(::CGraphWindow* parent)
	{
		QSplitter* centralWidget = new QSplitter;

		plot = new CGraphWidget(parent);
		plot->setObjectName("plot");

		centralWidget->addWidget(plot);
		centralWidget->addWidget(tools = new QToolBox); tools->hide();
		parent->setCentralWidget(centralWidget);

		ops = new OptionsUi(plot); ops->setObjectName("options");
		tools->addItem(ops, "Options");
		plot->addTool(ops);

		CPlotTool* tool = new RegressionUi(plot);
		tools->addItem(tool, "Linear Regression");
		plot->addTool(tool);

		toolBar = new QToolBar(parent);
		{
			selectPlot = new QComboBox;
			selectPlot->setObjectName("selectPlot");
			selectPlot->addItem("Line");
			selectPlot->addItem("Scatter");
			selectPlot->addItem("Time-Scatter");

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

#ifdef __APPLE__
CGraphWindow::CGraphWindow(CMainWindow* pwnd) : m_wnd(pwnd), QMainWindow(pwnd, Qt::WindowStaysOnTopHint), ui(new Ui::CGraphWindow)
#else
CGraphWindow::CGraphWindow(CMainWindow* pwnd) : m_wnd(pwnd), QMainWindow(pwnd), ui(new Ui::CGraphWindow)
#endif
{
	m_nTrackTime = TRACK_TIME;
	m_nUserMin = 0;
	m_nUserMax = -1;

	m_firstState = -1;
	m_lastState = -1;

	m_dataX = -1;
	m_dataY = -1;
	m_dataXPrev = -1;
	m_dataYPrev = -1;

	m_xtype = m_xtypeprev = -1;

	ui->setupUi(this);
	ui->ops->setUserRange(m_nUserMin, m_nUserMax);
	setMinimumWidth(500);
	resize(800, 600);
}

//-----------------------------------------------------------------------------
// If breset==true, a new model was loaded. 
// If breset==false, the selection has changed
void CGraphWindow::Update(bool breset, bool bfit)
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
	if (m_nTrackTime == TRACK_USER_RANGE)
	{
		// get the user defined range
		nmin = m_nUserMin;
		nmax = m_nUserMax;
	}
	else if (m_nTrackTime == TRACK_TIME)
	{
		TIMESETTINGS& timeSettings = doc->GetTimeSettings();
		nmin = timeSettings.m_start;
		nmax = timeSettings.m_end;
	}
	else if (m_nTrackTime == TRACK_CURRENT_TIME)
	{
		// simply set the min and max to the same value
		nmin = nmax = ntime;
	}

	// validate range
	if (nmin <       0) nmin = 0;
	if (nmax ==     -1) nmax = nsteps - 1;
	if (nmax >= nsteps) nmax = nsteps - 1;
	if (nmax <    nmin) nmax = nmin;

	// plot type
	int ntype = ui->selectPlot->currentIndex();
	int ncx = ui->selectTime->currentIndex();
	switch (ntype)
	{
	case LINE_PLOT: m_xtype = ncx; break;
	case SCATTER_PLOT: m_xtype = 2; break;
	case TIME_SCATTER_PLOT: m_xtype = 3; break;
	}

	// get the field data
	m_dataX = ui->selectX->currentValue();
	m_dataY = ui->selectY->currentValue();
	if ((ntype!=LINE_PLOT) && (m_dataX<=0))
	{
		ui->plot->clear();
		return;
	}
	if (m_dataY<=0) return;

	// When a reset is not required, see if we actually need to update anything
	if ((breset == false) && (bfit == false))
	{
		if ((nmin == m_firstState) && (nmax == m_lastState) && (m_dataX == m_dataXPrev) && (m_dataY == m_dataYPrev) && (m_xtype == m_xtypeprev)) return;
	}

	// set current time point index (TODO: Not sure if this is still used)
//	pview->SetCurrentTimeIndex(ntime);

	CGLModel* po = doc->GetGLModel();
	FEModel& fem = *doc->GetFEModel();

	FEMeshBase& mesh = *doc->GetFEModel()->GetFEMesh(0);

	// get the title
	if (ntype == LINE_PLOT)
	{
		ui->plot->setTitle(ui->selectY->text());
	}
	else
	{
		QString xtext = ui->selectX->text();
		QString ytext = ui->selectY->text();

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

	// get the graph of the track view and clear it
	ui->plot->clear();

	// add selections
	addSelectedNodes();
	addSelectedEdges();
	addSelectedFaces();
	addSelectedElems();

	// redraw
	if ((m_dataX != m_dataXPrev) || (m_dataY != m_dataYPrev) || (m_xtype != m_xtypeprev) || bfit)
	{
		ui->plot->fitToData();
	}

	m_dataXPrev = m_dataX;
	m_dataYPrev = m_dataY;
	m_xtypeprev = m_xtype;

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
	switch (m_xtype)
	{
	case 0: // time values
		{
			for (int i=0; i<NN; i++)
			{
				FENode& node = mesh.Node(i);
				if (node.IsSelected())
				{
					for (int j=0; j<nsteps; j++) xdata[j] = fem.GetState(j + m_firstState)->m_time;

					// evaluate y-field
					TrackNodeHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

					CPlotData plot;
					plot.setLabel(QString("N%1").arg(i+1));
					for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
					ui->plot->addPlotData(plot);
				}
			}
		}
		break;
	case 1: // step values
		{
			for (int i = 0; i<NN; i++)
			{
				FENode& node = mesh.Node(i);
				if (node.IsSelected())
				{
					for (int j = 0; j<nsteps; j++) xdata[j] = (float)j + 1.f + m_firstState;

					// evaluate y-field
					TrackNodeHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

					CPlotData plot;
					plot.setLabel(QString("N%1").arg(i + 1));
					for (int j = 0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
					ui->plot->addPlotData(plot);
				}
			}
		}
		break;
	case 2: // scatter
		{
			for (int i=0; i<NN; i++)
			{
				FENode& node = mesh.Node(i);
				if (node.IsSelected())
				{
					TrackNodeHistory(i, &xdata[0], m_dataX, m_firstState, m_lastState);

					// evaluate y-field
					TrackNodeHistory(i, &ydata[0], m_dataY, m_firstState, m_lastState);

					CPlotData plot;
					plot.setLabel(QString("N%1").arg(i+1));
					for (int j=0; j<nsteps; ++j) plot.addPoint(xdata[j], ydata[j]);
					ui->plot->addPlotData(plot);
				}
			}
		}
		break;
	case 3: // time-scatter
		{
			int nsteps = m_lastState - m_firstState + 1;
			if (nsteps > 32) nsteps = 32;
			for (int i=m_firstState; i<m_firstState + nsteps; ++i)
			{
				CPlotData plot;
				plot.setLabel(QString("%1").arg(fem.GetState(i)->m_time));
				ui->plot->addPlotData(plot);
			}

			for (int i = 0; i<NN; i++)
			{
				FENode& node = mesh.Node(i);
				if (node.IsSelected())
				{
					// evaluate x-field
					TrackNodeHistory(i, &xdata[0], m_dataX, m_firstState, m_firstState + nsteps -1);

					// evaluate y-field
					TrackNodeHistory(i, &ydata[0], m_dataY, m_firstState, m_firstState + nsteps -1);

					for (int j=0; j<nsteps; ++j)
					{
						CPlotData& p = ui->plot->getPlotData(j);
						p.addPoint(xdata[j], ydata[j]);
					}
				}
			}

			// sort the plots 
			int nplots = ui->plot->plots();
			for (int i=0; i<nplots; ++i)
			{
				CPlotData& data = ui->plot->getPlotData(i);
				data.sort();
			}
		}
		break;
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
void CGraphWindow::on_selectX_currentValueChanged(int)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectY_currentValueChanged(int)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CGraphWindow::on_selectPlot_currentIndexChanged(int index)
{
	if (index == 0)
		ui->selectXSource->setCurrentIndex(0);
	else 
		ui->selectXSource->setCurrentIndex(1);
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

//-----------------------------------------------------------------------------
void CGraphWindow::on_options_optionsChanged()
{
	int a = ui->ops->currentOption();
	switch (a)
	{
	case 0: m_nTrackTime = TRACK_TIME; break;
	case 1: m_nTrackTime = TRACK_CURRENT_TIME; break;
	case 2: m_nTrackTime = TRACK_USER_RANGE; 
		{
			ui->ops->getUserRange(m_nUserMin, m_nUserMax);
			ui->ops->setUserRange(m_nUserMin, m_nUserMax);
		}
		break;
	default:
		assert(false);
		m_nTrackTime = TRACK_TIME;
	}

	bool smooth = ui->ops->lineSmoothing();
	ui->plot->setLineSmoothing(smooth);

	bool marks = ui->ops->showDataMarks();
	ui->plot->showDataMarks(marks);

	Update(true);
}
