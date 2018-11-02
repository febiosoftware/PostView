#pragma once
#include <QDialog>

class CDlgExportVTK_UI;

class CDlgExportVTK : public QDialog
{
public:
	CDlgExportVTK(QWidget* parent);

	void accept();

public:
	bool	m_ops[2];

private:
	CDlgExportVTK_UI*	ui;
};
