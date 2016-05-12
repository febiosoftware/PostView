#pragma once
#include <QDialog>

namespace Ui{
	class CDlgBoxProps;
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
