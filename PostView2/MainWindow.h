/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once
#include <QMainWindow>
#include <QtCore/QBasicTimer>
#include <QCloseEvent>
#include "FileThread.h"
#include "ViewSettings.h"
#include "Document.h"

class CGLView;
class CFileThread;
class CTimePanel;
class CDocManager;
class CGraphWindow;

namespace Post {
	class FEFileReader;
}

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

	void OpenSession(const QString& fileName);

	CDocument* GetActiveDocument();

	CDocument* NewDocument(const std::string& docTitle);

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

	// update all the command panels
	void UpdateCommandPanels(bool breset, QWidget* psender = 0);

	// upate the Model Viewer
	void UpdateModelViewer(bool breset);

	// Build the context menu
	// Called when user right-clicks on OpenGL view
	QMenu* BuildContextMenu();

	// update the main toolbar
	// (e.g. when the field drop down needs to be updated)
	void UpdateMainToolbar(bool breset = true);

	// update the font toolbar
	// (e.g. when a GL widget gets selected)s
	void UpdateFontToolbar();

	// update play tool bar
	// (e.g. when the current time step is changed)
	void UpdatePlayToolbar(bool breset);

	// set the current time
	void SetCurrentTime(int n);
	void SetCurrentTimeValue(float ftime);

	// update the graph windows
	// (e.g. when selection has changed)
	void UpdateGraphs(bool breset = true, bool bfit = false);

	// update the tool window
	// (e.g. when selection was changed)
	void UpdateTools(bool reset = false);

	// set the data field on the toolbar
	void SetCurrentDataField(int nfield, bool blockSignal = false);

	// set a message on the status bar
	void SetStatusMessage(const QString& message);

	// clear the status message
	void ClearStatusMessage();

	// update status message (used for printing selection info)
	void UpdateStatusMessage();

	// sets the current folder
	void SetCurrentFolder(const QString& folder);

	// Set the window title
	void SetWindowTitle(const QString& t);

	// get the current them
	int currentTheme();
	
	// set current theme (must restart to take effect)
	void setCurrentTheme(int n);

	// get the view settings
	VIEWSETTINGS& GetViewSettings() { return m_ops; }

	// show data in a graph window
	void ShowData(const std::vector<double>& data, const QString& label);

	// remove a graph from the list
	void RemoveGraph(CGraphWindow* graph);

	// Add a graph to the list of managed graph windows
	void AddGraph(CGraphWindow* graph);

private:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

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

	void on_actionSelectConn_toggled(bool b);

	void on_actionZoomSelected_triggered();
	void on_actionZoomExtents_triggered();

	void on_actionPlaneCut_triggered();
	void on_actionMirrorPlane_triggered();
	void on_actionVectorPlot_triggered();
	void on_actionTensorPlot_triggered();
	void on_actionIsosurfacePlot_triggered();
	void on_actionSlicePlot_triggered();
	void on_actionDisplacementMap_triggered();
	void on_actionStreamLinePlot_triggered();
	void on_actionParticleFlowPlot_triggered();
	void on_actionVolumeFlowPlot_triggered();
	void on_actionImageSlicer_triggered();
	void on_actionVolumeRender_triggered();
	void on_actionMarchingCubes_triggered();
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
	void on_selectData_currentValueChanged(int i);
	void on_actionPlay_toggled(bool bchecked);
	void on_actionFirst_triggered();
	void on_actionPrev_triggered();
	void on_actionNext_triggered();
	void on_actionLast_triggered();
	void on_actionTimeSettings_triggered();

	void on_actionViewSettings_triggered();
	void on_actionViewCapture_toggled(bool bchecked);
	void on_actionViewWidgets_toggled(bool bchecked);
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
	void on_actionSyncViews_triggered();

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
	void on_selectAngle_valueChanged(int i);

	void on_tab_currentChanged(int i);
	void on_tab_tabCloseRequested(int i);

	void finishedReadingFile(bool success, const QString& errorString);

	void checkFileProgress();

	void onCancelFileRead();

	void on_recentFiles_triggered(QAction* action);
	void on_recentSessions_triggered(QAction* action);

	void onTimer();

	void onAppLoadFile(const QString& fileName);

private:
	void closeEvent(QCloseEvent* ev) override;
	void StopAnimation();

	void AddDocument(CDocument* doc);
	void AddDocumentTab(CDocument* doc);

	void MakeDocActive(CDocument* doc);

	void keyPressEvent(QKeyEvent* ev) override;

private:
	void writeSettings();
	void readSettings();

private:
	Ui::CMainWindow*	ui;
	CFileThread*		m_fileThread;
	VIEWSETTINGS		m_ops;		// the view settings

	CDocManager*		m_DocManager;
	CDocument*			m_activeDoc;
};
