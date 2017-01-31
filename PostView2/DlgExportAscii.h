#pragma once
#include <QDialog>

namespace Ui {
	class CDlgExportAscii;
};

class CDlgExportAscii : public QDialog
{
public:
	CDlgExportAscii(QWidget* parent);
	
	void accept();

public:
	bool	m_bcoords;
	bool	m_belem;
	bool	m_bndata;
	bool	m_bedata;
	bool	m_bface;
	bool	m_bsel;
	bool	m_bfnormals;

	int		m_nstep;	 // 0 = current, 1 = all

	char	m_szfmt[256];	// format string for data values

private:
	Ui::CDlgExportAscii* ui;
};
