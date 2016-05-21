#include "stdafx.h"
#include "SummaryWindow.h"
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

class Ui::CSummaryWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			toolBar;
	QToolBar*			zoomBar;
	CDataFieldSelector*	selectData;

	QAction*	actionZoomSelect;
	QCheckBox*	selectionOnly;
	QCheckBox*	volumeAverage;

public:
	void setupUi(QMainWindow* parent)
	{
		plot = new CPlotWidget(parent);
		plot->setObjectName("summaryPlot");
		parent->setCentralWidget(plot);

		toolBar = new QToolBar(parent);
		QAction* actionSave = toolBar->addAction(QIcon(QString(":/icons/save.png")), "Save"); actionSave->setObjectName("actionSave");
		QAction* actionClip = toolBar->addAction(QIcon(QString(":/icons/clipboard.png")), "Copy to clipboard"); actionClip->setObjectName("actionClip");
		toolBar->addWidget(new QLabel("Data: "));

		selectData = new CDataFieldSelector;
		selectData->setObjectName("selectData");
		selectData->setMinimumWidth(150);
		toolBar->addWidget(selectData);
		toolBar->addSeparator();
		toolBar->addWidget(selectionOnly = new QCheckBox("Selection only")); selectionOnly->setObjectName("selectionOnly");
		toolBar->addWidget(volumeAverage = new QCheckBox("Volume average")); volumeAverage->setObjectName("volumeAverage");

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

CSummaryWindow::CSummaryWindow(CMainWindow* wnd) : m_wnd(wnd), QMainWindow(wnd), ui(new Ui::CSummaryWindow)
{
	m_ncurrentData = -1;

	ui->setupUi(this);
	setMinimumWidth(500);
	resize(600, 500);
}

void CSummaryWindow::Update(bool breset)
{
	CDocument* doc = m_wnd->GetDocument();
	if (breset)
	{
		if (doc->IsValid())
		{
			ui->selectData->BuildMenu(doc->GetFEModel(), DATA_SCALAR);
		}
		else return;
	}

	// get the selection mode
	int nmode = doc->GetSelectionMode();

	CGLModel* po = doc->GetGLModel();
	FEModel* pfem = doc->GetFEModel();
	FEMesh* pfe = doc->GetFEModel()->GetMesh();
	int nodes = pfe->Nodes();

	// get the current data field
	int ndata = ui->selectData->currentValue();
	if (ndata <= 0) return;

	// only update if the data was changed
	if ((ndata == m_ncurrentData) && (breset == false)) return;
	m_ncurrentData = ndata;

	// get the text and make it the plot title
	ui->plot->setTitle(ui->selectData->currentText());

	// see if selection only box is checked
	bool bsel = ui->selectionOnly->isChecked();

	// see if volume average is checked
	bool bvol = ui->volumeAverage->isChecked();

	// decide if we want to find the node/face/elem stat
	int neval = -1;
	if ((bsel && (nmode == SELECT_NODES)) || IS_NODE_FIELD(m_ncurrentData)) neval = 0;
	if ((bsel && (nmode == SELECT_EDGES)) || IS_EDGE_FIELD(m_ncurrentData)) neval = 1;
	if ((bsel && (nmode == SELECT_FACES)) || IS_FACE_FIELD(m_ncurrentData)) neval = 2;
	if ((bsel && (nmode == SELECT_ELEMS)) || IS_ELEM_FIELD(m_ncurrentData)) neval = 3;
	assert(neval >= 0);

	// clear the graph
	ui->plot->clear();

	// copy the nodal values
	vector<float> val(nodes);
	int nfield;

	// get the number of time steps
	int nsteps = doc->GetTimeSteps();

	// allocate data
	vector<double> x(nsteps);
	// add the data series
	for (int i=0; i<nsteps; i++) x[i] = pfem->GetState(i)->m_time;

	CPlotData dataMax, dataMin, dataAvg;
	dataMax.setLabel("Max");
	dataAvg.setLabel("Avg");
	dataMin.setLabel("Min");

	// get the displacemet map
	CGLDisplacementMap* pdm = po->GetDisplacementMap();

	// loop over all time steps
	for (int i=0; i<nsteps; i++)
	{
		// get the state
		FEState* ps = pfem->GetState(i);

		// we need to make sure that the displacements are updated
		// in case the user evaluates the strains
		if (pdm) pdm->UpdateState(i);

		// store the nodal values and field
		nfield = ps->m_nField;
		for (int j=0; j<nodes; ++j) val[j] = ps->m_NODE[j].m_val;

		// evaluate the mesh
		pfem->Evaluate(m_ncurrentData, i);
 
		// store min/max values
		RANGE rng;
		switch (neval)
		{
		case 0: rng = EvalNodeRange(*pfem, i, bsel); break;
		case 1: rng = EvalEdgeRange(*pfem, i, bsel); break;
		case 2: rng = EvalFaceRange(*pfem, i, bsel, bvol); break;
		case 3: rng = EvalElemRange(*pfem, i, bsel, bvol); break;
		}
		dataMax.addPoint(x[i], rng.fmax);
		dataMin.addPoint(x[i], rng.fmin);
		dataAvg.addPoint(x[i], rng.favg);
		
		// reset the field data
		if (nfield >= 0) pfem->Evaluate(nfield, i);
	}

	// add the data
	ui->plot->addPlotData(dataMax);
	ui->plot->addPlotData(dataAvg);
	ui->plot->addPlotData(dataMin);

	// redraw
	ui->plot->fitToData();
	ui->plot->repaint();
}

//-----------------------------------------------------------------------------
// Evaluate the range of unpacked nodal data values
CSummaryWindow::RANGE CSummaryWindow::EvalNodeRange(FEModel& fem, int nstate, bool bsel)
{
	RANGE rng = {-1e20f, 1e20f, 0.f};

	FEState& state = *fem.GetState(nstate);
	FEMesh& mesh = *fem.GetMesh();

	float sum = 0;
	
	int NN = mesh.Nodes();
	for (int i=0; i<NN; i++)
	{
		FENode& node = mesh.Node(i);
		if ((bsel == false) || (node.IsSelected()))
		{
			float val = state.m_NODE[i].m_val;
			rng.favg += val;
			sum += 1.f;
			if (val > rng.fmax) rng.fmax = val;
			if (val < rng.fmin) rng.fmin = val;
		}
	}

	if (sum == 0.f) rng.fmin = rng.fmax = 0.f;
	else rng.favg /= sum;

	return rng;
}

//-----------------------------------------------------------------------------
// Evaluate the range of unpacked nodal data values
CSummaryWindow::RANGE CSummaryWindow::EvalEdgeRange(FEModel& fem, int nstate, bool bsel)
{
	RANGE rng = {-1e20f, 1e20f, 0.f};

	FEState& state = *fem.GetState(nstate);
	FEMesh& mesh = *fem.GetMesh();

	float sum = 0;
	
	int NE = mesh.Edges();
	for (int i=0; i<NE; i++)
	{
		FEEdge& edge = mesh.Edge(i);
		if ((bsel == false) || (edge.IsSelected()))
		{
			float val = state.m_EDGE[i].m_val;
			rng.favg += val;
			sum += 1.f;
			if (val > rng.fmax) rng.fmax = val;
			if (val < rng.fmin) rng.fmin = val;
		}
	}

	if (sum == 0.f) rng.fmin = rng.fmax = 0.f;
	else rng.favg /= sum;

	return rng;
}

//-----------------------------------------------------------------------------
// Evaluate the range of unpacked element data values
CSummaryWindow::RANGE CSummaryWindow::EvalElemRange(FEModel& fem, int nstate, bool bsel, bool bvol)
{
	RANGE rng = {-1e20f, 1e20f, 0.f};

	FEState& state = *fem.GetState(nstate);
	FEMesh& mesh = *fem.GetMesh();

	float sum = 0.f;
	int NE = mesh.Elements();
	for (int i=0; i<NE; i++)
	{
		FEElement& e = mesh.Element(i);

		if ((bsel == false) || (e.IsSelected()))
		{
			float val = state.m_ELEM[i].m_val;
			float w = 1;
			if (bvol) w = mesh.ElementVolume(i);

			rng.favg += val*w;
			sum += w;
			if (val > rng.fmax) rng.fmax = val;
			if (val < rng.fmin) rng.fmin = val;
		}
	}

	if (sum == 0.f) rng.fmin = rng.fmax = 0.f;
	else rng.favg /= sum;

	return rng;
}

//-----------------------------------------------------------------------------
// Evaluate the range of unpacked face data values
CSummaryWindow::RANGE CSummaryWindow::EvalFaceRange(FEModel& fem, int nstate, bool bsel, bool bvol)
{
	RANGE rng = {-1e20f, 1e20f, 0.f};

	FEState& state = *fem.GetState(nstate);
	FEMesh& mesh = *fem.GetMesh();

	float sum = 0.f;
	int NF = mesh.Faces();
	for (int i=0; i<NF; i++)
	{
		FEFace& f = mesh.Face(i);

		if ((bsel == false) || (f.IsSelected()))
		{
			float val = state.m_FACE[i].m_val;
			float w = 1;
			if (bvol) w = mesh.FaceArea(f);

			rng.favg += val*w;
			sum += w;
			if (val > rng.fmax) rng.fmax = val;
			if (val < rng.fmin) rng.fmin = val;
		}
	}

	if (sum == 0.f) rng.fmin = rng.fmax = 0.f;
	else rng.favg /= sum;

	return rng;
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_selectData_currentIndexChanged(int index)
{
	Update(false);
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Summary Data", QDir::currentPath(), QString("All files (*)"));
	if (fileName.isEmpty() == false)
	{
		if (ui->plot->Save(fileName) == false)
			QMessageBox::critical(this, "Save Summary Data", "A problem occurred saving the data.");
	}
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionClip_triggered()
{
	ui->plot->OnCopyToClipboard();
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionProps_triggered()
{
	ui->plot->OnShowProps();
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionZoomWidth_triggered()
{
	ui->plot->OnZoomToWidth();
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionZoomHeight_triggered()
{
	ui->plot->OnZoomToHeight();
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionZoomFit_triggered()
{
	ui->plot->OnZoomToFit();
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_actionZoomSelect_toggled(bool bchecked)
{
	ui->plot->ZoomToRect(bchecked);
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_summaryPlot_doneZoomToRect()
{
	ui->actionZoomSelect->setChecked(false);
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_selectionOnly_clicked()
{
	m_ncurrentData = -1;
	Update(false);
}

//-----------------------------------------------------------------------------
void CSummaryWindow::on_volumeAverage_clicked()
{
	m_ncurrentData = -1;
	Update(false);
}
