#pragma once
#include <QDialog>

class CDlgExportLSDYNA_UI;

class CDlgExportLSDYNA : public QDialog
{
public:
	CDlgExportLSDYNA(QWidget* parent);

	void accept();

public:
	bool	m_bsel;
	bool	m_bsurf;
	bool	m_bnode;

private:
	CDlgExportLSDYNA_UI*	ui;
};
