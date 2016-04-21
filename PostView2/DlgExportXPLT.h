#pragma once
#include <QDialog>

namespace Ui {
	class CDlgExportXPLT;
};

class CMainWindow;

class CDlgExportXPLT : public QDialog
{
	Q_OBJECT

public:
	CDlgExportXPLT(CMainWindow* pwnd);

public slots:
	void accept();

public:
	bool	m_bcompress;

protected:
	Ui::CDlgExportXPLT*	ui;
};
