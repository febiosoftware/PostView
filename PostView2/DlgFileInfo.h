#pragma once
#include <QDialog>

namespace Ui {
	class CDlgFileInfo;
}

class CDlgFileInfo : public QDialog
{
public:
	CDlgFileInfo(QWidget* parent = 0);

	void setSoftware(const QString& n);

private:
	Ui::CDlgFileInfo*	ui;
};
