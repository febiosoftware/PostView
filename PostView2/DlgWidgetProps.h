#pragma once
#include <QDialog>

namespace Ui{
	class CDlgBoxProps;
	class CDlgLegendProps;
	class CDlgTriadProps;
	class CDlgCaptureFrameProps;
};

class GLWidget;
class QAbstractButton;

class CDlgBoxProps : public QDialog
{
	Q_OBJECT

public:
	CDlgBoxProps(GLWidget* widget, QWidget* parent);

	void accept();
	void apply();

private slots:
	void onClicked(QAbstractButton* button);

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
	void apply();

private slots:
	void onClicked(QAbstractButton* button);

private:
	Ui::CDlgLegendProps* ui;
	GLWidget* pw;
};

class CDlgTriadProps : public QDialog
{
	Q_OBJECT

public:
	CDlgTriadProps(GLWidget* widget, QWidget* parent);

	void accept();
	void apply();

private slots:
	void onClicked(QAbstractButton* button);

private:
	Ui::CDlgTriadProps* ui;
	GLWidget* pw;
};

class CDlgCaptureFrameProps : public QDialog
{
	Q_OBJECT

public:
	CDlgCaptureFrameProps(GLWidget* widget, QWidget* parent);

	void accept();
	void apply();

private slots:
	void onClicked(QAbstractButton* button);

private slots:
	void onFormat(int nindex);

private:
	Ui::CDlgCaptureFrameProps* ui;
	GLWidget* pw;
};
