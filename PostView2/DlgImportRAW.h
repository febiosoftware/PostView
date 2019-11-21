#pragma once
#include <QDialog>

namespace Ui {
	class CDlgImportRAW;
}

class CDlgImportRAW : public QDialog
{
public:
	CDlgImportRAW(QWidget* parent);

private:
	void accept() override;

public:
	int		m_nx, m_ny, m_nz;
	double	m_min[3], m_max[3];

	Ui::CDlgImportRAW*	ui;
};
