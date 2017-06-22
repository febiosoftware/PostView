#pragma once
#include <QMainWindow>
#include <QtCore/QBasicTimer>
#include <QCloseEvent>
#include <QtCore/QThread>

class CDocument;
class CGLView;
class FEFileReader;
class CFileThread;
class CTimePanel;

namespace Ui {
	class CMainWindow;
}

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CMainWindow(QWidget* parent = 0);
	~CMainWindow();

	void OpenFile(const QString& fileName, int nfilter);
	bool SaveFile(const QString& fileName, int nfilter);

	CDocument*	GetDocument() { return m_doc; }

	CGLView* GetGLView();

	void UpdateView();

	// force a redraw of the GLView
	void RedrawGL();

	// checks the Ui and makes any necessary adjustments
	// TODO: use this instead of the UpdateUi stuff
	void CheckUi();

	// this is called after a new model is loaded 
	// or an existing model is reloaded
	// and also after a mouse release event
	void UpdateUi(bool breset, QWidget* psender = 0);

	// Build the context menu
	// Called when user right-clicks on OpenGL view
	QMenu* BuildContextMenu();

	// update the main toolbar
	// (e.g. when the field drop down needs to be updated)
	void UpdateMainToolbar();

	// update the font toolbar
	// (e.g. when a GL widget gets selected)s
	void UpdateFontToolbar();

	// update play tool bar
	// (e.g. when the current time step is changed)
	void UpdatePlayToolbar(bool breset);

	// set the current time
	void SetCurrentTime(int n);

	// update the graph windows
	// (e.g. when selection has changed)
	void UpdateGraphs(bool breset = true, bool bfit = false);

	// update the tool window
	// (e.g. when selection was changed)
	void UpdateTools(bool reset = false);

	// set the data field on the toolbar
	void SetCurrentDataField(int nfield);

	// set a message on the status bar
	void SetStatusMessage(const QString& message);

	// clear the status message
	void ClearStatusMessage();

	// update status message (used for printing selection info)
	void UpdateStatusMessage();

	// sets the current folder
	void SetCurrentFolder(const QString& folder);

public slots:
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionUpdate_triggered();
	void on_actionFileInfo_triggered();
	void on_actionSnapShot_triggered();
	void on_actionOpenSession_triggered();
	void on_actionSaveSession_triggered();
	void on_actionQuit_triggered();

	void on_selectNodes_triggered();
	void on_selectEdges_triggered();
	void on_selectFaces_triggered();
	void on_selectElems_triggered();

	void on_actionSelectRect_triggered();
	void on_actionSelectCircle_triggered();
	void on_actionSelectFree_triggered();

	void on_actionSelectItem_triggered();
	void on_actionSelectConn_triggered();

	void on_actionZoomSelected_triggered();
	void on_actionZoomExtents_triggered();

	void on_actionPlaneCut_triggered();
	void on_actionVectorPlot_triggered();
	void on_actionIsosurfacePlot_triggered();
	void on_actionSlicePlot_triggered();
	void on_actionDisplacementMap_triggered();
	void on_actionGraph_triggered();
	void on_actionSummary_triggered();
	void on_actionStats_triggered();
	void on_actionIntegrate_triggered();

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
	void on_actionTimeSettings_triggered();

	void on_actionViewSettings_triggered();
	void on_actionViewCapture_toggled(bool bchecked);
	void on_actionViewProjection_toggled(bool bchecked);
	void on_actionViewFront_triggered();
	void on_actionViewBack_triggered();
	void on_actionViewLeft_triggered();
	void on_actionViewRight_triggered();
	void on_actionViewTop_triggered();
	void on_actionViewBottom_triggered();
	void on_actionViewMesh_toggled(bool bchecked);
	void on_actionViewOutline_toggled(bool bchecked);
	void on_actionViewShowTags_toggled(bool bchecked);
	void on_actionViewSmooth_toggled(bool bchecked);
	void on_actionViewTrack_toggled(bool bchecked);
	void on_actionViewVPSave_triggered();
	void on_actionViewVPPrev_triggered();
	void on_actionViewVPNext_triggered();

	void on_actionRecordNew_triggered();
	void on_actionRecordStart_triggered();
	void on_actionRecordPause_triggered();
	void on_actionRecordStop_triggered();

	void on_actionHelp_triggered();
	void on_actionAbout_triggered();

	void on_fontStyle_currentFontChanged(const QFont& font);
	void on_fontSize_valueChanged(int i);
	void on_fontBold_toggled(bool checked);
	void on_fontItalic_toggled(bool bchecked);

	void on_selectTime_valueChanged(int i);

	void finishedReadingFile(bool success, const QString& errorString);

	void checkFileProgress();

	void onCancelFileRead();

private:
	void timerEvent(QTimerEvent* ev);
	void closeEvent(QCloseEvent* ev);
	void StopAnimation();


private:
	void writeSettings();
	void readSettings();

private:
	Ui::CMainWindow*	ui;
	CDocument*			m_doc;
	QBasicTimer			m_timer;
	CFileThread*		m_fileThread;
};

class CFileThread : public QThread
{
	Q_OBJECT

	void run() Q_DECL_OVERRIDE;

public:
	CFileThread(CMainWindow* wnd, FEFileReader* file, const QString& fileName);

	float getFileProgress() const;

	FEFileReader* GetFileReader() { return m_fileReader; }

signals:
	void resultReady(bool, const QString&);

private:
	CMainWindow*	m_wnd;
	FEFileReader*	m_fileReader;
	QString			m_fileName;
};
