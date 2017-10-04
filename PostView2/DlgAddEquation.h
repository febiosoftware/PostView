#pragma once
#include <QDialog>

class CDlgAddEquation_UI;

class CDlgAddEquation : public QDialog
{
public:
	CDlgAddEquation(QWidget* parent = 0);

	QString GetDataName();
	QString GetEquation();

private:
	CDlgAddEquation_UI*	ui;
};
