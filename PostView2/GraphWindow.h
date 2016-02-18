#pragma once
#include <QMainWindow>

namespace Ui {
	class CGraphWindow;
};

class CGraphWindow : public QMainWindow
{
	Q_OBJECT

public:
	CGraphWindow(QWidget* parent);

private:
	Ui::CGraphWindow*	ui;
};
