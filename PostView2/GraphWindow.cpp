#include "GraphWindow.h"
#include "PlotWidget.h"

class Ui::CGraphWindow
{
public:
	CPlotWidget*	plot;
public:
	void setupUi(::CGraphWindow* parent)
	{
		plot = new CPlotWidget(parent);
		parent->setCentralWidget(plot);
	}
};

CGraphWindow::CGraphWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CGraphWindow)
{
	ui->setupUi(this);
	resize(400, 300);
}
