#include "stdafx.h"
#include "StatsWindow.h"
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

class Ui::CStatsWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			toolBar;

public:
	void setupUi(QMainWindow* parent)
	{
		plot = new CPlotWidget(parent);
		plot->setObjectName("statsPlot");
		plot->showLegend(false);
		plot->setViewLocked(true);
		plot->showPopup(false);
		plot->showVerticalGridLines(false);
		plot->showXAxis(false);
		plot->ShowYAxis(false);
		plot->setChartStyle(CPlotWidget::BarChart);
		parent->setCentralWidget(plot);

		toolBar = new QToolBar(parent);
		QAction* actionSave = toolBar->addAction(QIcon(QString(":/icons/save.png")), "Save"); actionSave->setObjectName("actionSave");
		QAction* actionClip = toolBar->addAction(QIcon(QString(":/icons/clipboard.png")), "Copy to clipboard"); actionClip->setObjectName("actionClip");

		parent->addToolBar(Qt::TopToolBarArea, toolBar);

		QMetaObject::connectSlotsByName(parent);
	}
};

CStatsWindow::CStatsWindow(CMainWindow* wnd) : m_wnd(wnd), QMainWindow(wnd), ui(new Ui::CStatsWindow)
{
	ui->setupUi(this);
	setMinimumWidth(500);
	resize(600, 500);
}

void CStatsWindow::Update(bool breset)
{
	CDocument* doc = m_wnd->GetDocument();
	if (doc->IsValid() == false) return;

	FEMesh* pm = doc->GetFEModel()->GetMesh();
	int N, i, n;

	bool belemfield = IS_ELEM_FIELD(doc->GetEvalField());

	N = 0;
	if (belemfield)
	{
		for (i=0; i<pm->Elements(); ++i) if (pm->Element(i).IsEnabled()) ++N;
	}
	else
	{
		for (i=0; i<pm->Nodes(); ++i) if (pm->Node(i).IsEnabled()) ++N;
	}

	int nbins = (int) sqrt((double)N);

	std::vector<int> bin(nbins, 0);

	FEState* ps = doc->GetGLModel()->currentState();
	double v, minv = 0, maxv = 0;
	N = 0;
	if (belemfield)
	{
		for (i=0; i<pm->Elements(); ++i)
		{
			FEElement& elem = pm->Element(i);
			if (elem.IsEnabled())
			{
				v = ps->m_ELEM[i].m_val;
				if (N==0) minv = maxv = v;
				if (v < minv) minv = v;
				if (v > maxv) maxv = v;
				++N;
			}
		}
	}
	else
	{
		for (i=0; i<pm->Nodes(); ++i)
		{	
			FENode& node = pm->Node(i);
			if (node.IsEnabled())
			{
				v = ps->m_NODE[i].m_val;
				if (N==0) minv = maxv = v;
				if (v < minv) minv = v;
				if (v > maxv) maxv = v;
				++N;
			}
		}
	}

	if (minv == maxv) ++maxv;

	if (belemfield)
	{
		for (i=0; i<pm->Elements(); ++i)
		{
			FEElement& elem = pm->Element(i);
			if (elem.IsEnabled())
			{
				v = ps->m_ELEM[i].m_val;
				n = (int) ((nbins-1)*((v - minv)/(maxv - minv)));
				if ((n>=0) && (n<nbins)) bin[n]++;
			}
		}
	}
	else
	{
		for (i=0; i<pm->Nodes(); ++i)
		{
			FENode& node = pm->Node(i);
			if (node.IsEnabled())
			{
				v = ps->m_NODE[i].m_val;
				n = (int) ((nbins-1)*((v - minv)/(maxv - minv)));
				if ((n>=0) && (n<nbins)) bin[n]++;
			}
		}
	}

	ui->plot->clear();
	
	if (bin.empty() == false)
	{
		CPlotData data;
		data.setLabel("data");
		for (int i=0; i<bin.size(); ++i)
		{
			double x = minv + i*(maxv - minv)/(bin.size() - 1);
			double y = bin[i];
			data.addPoint(x, y);
		}
		ui->plot->addPlotData(data);

		UpdateSelection(true);
	}

	// redraw
	ui->plot->fitToData();
	ui->plot->repaint();
}

void CStatsWindow::UpdateSelection(bool breset)
{
/*	double fmin, fmax;
	if (breset)
	{
		m_pBar->GetRange(fmin, fmax);
		m_pmin->value((float) fmin);
		m_pmax->value((float) fmax);
	}
	else
	{
		fmin = m_pmin->value();
		fmax = m_pmax->value();
	}

	CDocument* pdoc = m_pWnd->GetDocument();

	int nfield = pdoc->GetEvalField();
	if (m_psel->value())
	{
		if (IS_ELEM_FIELD(nfield))
		{
			pdoc->SelectElemsInRange((float) fmin, (float) fmax, false);
		}
		else if (IS_NODE_FIELD(nfield))
		{
			pdoc->SelectNodesInRange((float) fmin, (float) fmax, false);
		}
	}
	else
	{
		pdoc->GetFEModel()->GetMesh()->ClearSelection();
	}

	m_pWnd->RedrawGL();
*/
}

//-----------------------------------------------------------------------------
void CStatsWindow::on_actionSave_triggered()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Save Stats Data", QDir::currentPath(), QString("All files (*)"));
	if (fileName.isEmpty() == false)
	{
		if (ui->plot->Save(fileName) == false)
			QMessageBox::critical(this, "Save Stats Data", "A problem occurred saving the data.");
	}
}

//-----------------------------------------------------------------------------
void CStatsWindow::on_actionClip_triggered()
{
	ui->plot->OnCopyToClipboard();
}
