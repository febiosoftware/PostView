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
		actionProps = tool->addAction(QIcon(QString(":/icons/properties.png")), "Properties"); actionProps->setObjectName("actionProps");

		zoomBar = new QToolBar(parent);
		QAction* actionZoomWidth  = zoomBar->addAction(QIcon(QString(":/icons/zoom_width.png" )), "Zoom Width" ); actionZoomWidth->setObjectName("actionZoomWidth" );
		QAction* actionZoomHeight = zoomBar->addAction(QIcon(QString(":/icons/zoom_height.png")), "Zoom Height"); actionZoomHeight->setObjectName("actionZoomHeight");
		QAction* actionZoomFit    = zoomBar->addAction(QIcon(QString(":/icons/zoom_fit.png"   )), "Zoom Fit"   ); actionZoomFit->setObjectName("actionZoomFit"   );

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
	if (breset)
	{
		if (doc->IsValid())
		{
			ui->selectX->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
			ui->selectY->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
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

//-----------------------------------------------------------------------------
void CGraphWindow::on_actionSave_triggered()
{
	CPlotWidget* pview = ui->plot;
	CDocument* pdoc = m_wnd->GetDocument();

	QString fileName = QFileDialog::getSaveFileName(this, "Save Graph Data", QDir::currentPath(), QString("All files (*)"));
	if (fileName.isEmpty() == false)
	{
		// dump the data to a text file
		std::string sfile = fileName.toStdString();
		FILE* fp = fopen(sfile.c_str(), "wt");
		if (fp == 0) 
		{
			QMessageBox::critical(this, "Save Graph Data", "A problem occurred saving the data.");
			return;
		}

		std::string title = pview->title().toStdString();

		fprintf(fp, "#Trackview output generated by PostView version %d.%d\n", VERSION, SUBVERSION);
		fprintf(fp, "#Title : %s\n", pdoc->GetTitle());
		fprintf(fp, "#Data : %s\n", title.c_str());

		fprintf(fp,"\n");
		CPlotData& plot = pview->getPlotData(0);
		for (int i=0; i<plot.size(); i++)
		{
			fprintf(fp, "%16.9g ", plot.Point(i).x());
			for (int j=0; j<pview->plots(); j++)
			{
				CPlotData& plotj = pview->getPlotData(j);
				fprintf(fp,"%16.9g ", plotj.Point(j).y());
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
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
