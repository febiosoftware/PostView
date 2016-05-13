#pragma once
#include <QDialog>

namespace Ui{
	class CDlgBoxProps;
	class CDlgLegendProps;
};

class GLWidget;

class CDlgBoxProps : public QDialog
{
	Q_OBJECT

public:
	CDlgBoxProps(GLWidget* widget, QWidget* parent);

	void accept();

private:
	Ui::CDlgBoxProps* ui;
	GLWidget* pw;
};

class CDlgLegendProps : public QDialog
{
	Q_OBJECT

public:
	CDlgLegendProps(GLWidget* widget, QWidget* parent);

	void accept();

private:
	Ui::CDlgLegendProps* ui;
	GLWidget* pw;
};
