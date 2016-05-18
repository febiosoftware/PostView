#pragma once
#include <QDialog>

namespace Ui {
	class CDlgTimeSettings;
};

class CDocument;

class CDlgTimeSettings : public QDialog
{
public:
	CDlgTimeSettings(CDocument* doc, QWidget* parent);

	void accept();

private:
	Ui::CDlgTimeSettings* ui;
	CDocument*	m_doc;
};
