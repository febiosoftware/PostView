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

	bool OpenFile(const QString& fileName, int nfilter);
	bool SaveFile(const QString& fileName, int nfilter);

	CDocument*	GetDocument() { return m_doc; }

private slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionUpdate_triggered();
	void on_actionSnapShot_triggered();
	void on_actionQuit_triggered();

	void on_selectNodes_triggered();
	void on_selectFaces_triggered();
	void on_selectElems_triggered();
	void on_actionGraph_triggered();

	void on_actionHideSelected_triggered();
	void on_actionHideUnselected_triggered();
	void on_actionInvertSelection_triggered();
	void on_actionUnhideAll_triggered();
	void on_actionSelectAll_triggered();
	void on_actionSelectRange_triggered();
	void on_actionClearSelection_triggered();
	void on_actionFind_triggered();
	void on_actionDelete_triggered();
	void on_actionProperties_triggered();

	void on_actionColorMap_toggled(bool bchecked);
	void on_selectData_currentIndexChanged(int i);
	void on_actionPlay_toggled(bool bchecked);
	void on_actionFirst_triggered();
	void on_actionPrev_triggered();
	void on_actionNext_triggered();
	void on_actionLast_triggered();

	void on_actionViewMesh_toggled(bool bchecked);
	void on_actionViewOutline_toggled(bool bchecked);
	void on_actionViewSmooth_toggled(bool bchecked);

private:
	void timerEvent(QTimerEvent* ev);

	// this is called after a new model is loaded 
	// or an existing model is reloaded
	void UpdateUi();

private:
	Ui::CMainWindow*	ui;
	CDocument*			m_doc;
	QBasicTimer			m_timer;
};
