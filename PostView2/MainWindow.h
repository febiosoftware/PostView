#pragma once
#include <QMainWindow>

class CDocument;

namespace Ui {
	class CMainWindow;
}

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CMainWindow(QWidget* parent = 0);
	~CMainWindow();

	bool OpenFile(const QString& fileName);

	CDocument*	GetDocument() { return m_doc; }

private slots:
	void on_actionOpen_triggered();
	void on_actionQuit_triggered();
	void on_actionColorMap_toggled(bool bchecked);
	void on_selectData_currentIndexChanged(int i);

private:
	Ui::CMainWindow*	ui;
	CDocument*	m_doc;
};
