#include "GraphWindow.h"
#include "PlotWidget.h"
#include "DataFieldSelector.h"
#include <QToolbar>
#include "MainWindow.h"
#include "Document.h"
#include <PostViewLib/FEModel.h>

class Ui::CGraphWindow
{
public:
	CPlotWidget*		plot;
	QToolBar*			tool;
	CDataFieldSelector*	select;

public:
	void setupUi(::CGraphWindow* parent)
	{
		plot = new CPlotWidget(parent);
		parent->setCentralWidget(plot);

		tool = new QToolBar(parent);
		select = new CDataFieldSelector;
		tool->addWidget(select);

		parent->addToolBar(Qt::TopToolBarArea, tool);
	}
};

CGraphWindow::CGraphWindow(CMainWindow* pwnd) : m_wnd(pwnd), QMainWindow(pwnd), ui(new Ui::CGraphWindow)
{
	ui->setupUi(this);
	resize(400, 300);
}

void CGraphWindow::Update()
{
	CDocument* doc = m_wnd->GetDocument();
	if (doc->IsValid())
	{
		ui->select->BuildMenu(doc->GetFEModel(), DATA_FLOAT);
	}
}
