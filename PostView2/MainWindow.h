#pragma once
#include <QMainWindow>
#include <QtCore/QBasicTimer>

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
	bool SaveFile(const QString& fileName, const QString& flt);

	CDocument*	GetDocument() { return m_doc; }

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionUpdate_triggered();
	void on_actionQuit_triggered();
	void on_actionColorMap_toggled(bool bchecked);
	void on_selectData_currentIndexChanged(int i);
	void on_actionPlay_toggled(bool bchecked);
	void on_actionFirst_triggered();
	void on_actionPrev_triggered();
	void on_actionNext_triggered();
	void on_actionLast_triggered();

private:
	void timerEvent(QTimerEvent* ev);

private:
	Ui::CMainWindow*	ui;
	CDocument*			m_doc;
	QBasicTimer			m_timer;
};
