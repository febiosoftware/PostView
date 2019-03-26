#pragma once
#include <QHBoxLayout>
#include <QMenuBar>
#include <QTextEdit>
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>
#include <QLabel>
#include "FileViewer.h"
#include "ModelViewer.h"
#include "MaterialPanel.h"
#include "DataPanel.h"
#include "StatePanel.h"
#include <QComboBox>
#include <QTreeWidget>
#include <QHeaderView>
#include "ToolsPanel.h"
#include "DataFieldSelector.h"
#include "GLView.h"
#include "GraphWindow.h"
#include "SummaryWindow.h"
#include "IntegrateWindow.h"
#include "StatsWindow.h"
#include "TimePanel.h"
#include <QFontComboBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QtCore/qdir.h>
#include <QPushButton>
#include <QToolButton>
#include <QWhatsThis>
#include <QTabBar>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
	enum 
	{
		wndWidth   = 800,		// width of main window
		wndHeight  = 600,		// height of main window
		menuHeight = 21			// height of menu bar
	};

	enum
	{
		MAX_RECENT_FILES = 15		// max number of recent files
	};

private:
	CMainWindow*	pwnd;

public:
	QMenu* menuFile;
	QMenu* menuEdit; 
	QMenu* menuPost; 
	QMenu* menuRecord;
	QMenu* menuView;
	QMenu* menuHelp; 
	QMenu* menuRecentFiles;

	QActionGroup* recentFilesActionGroup;

	QAction* actionViewSettings;
	QAction* actionViewCapture; 
	QAction* actionViewWidgets;
	QAction* actionViewProjection;
	QAction* actionViewMesh;
	QAction* actionViewOutline;
	QAction* actionViewSmooth;  
	QAction* actionViewFront;   
	QAction* actionViewBack;    
	QAction* actionViewLeft;     
	QAction* actionViewRight;     
	QAction* actionViewTop;    
	QAction* actionViewBottom;
	QAction* actionViewTrack;   
	QAction* actionViewVPSave;    
	QAction* actionViewVPPrev;   
	QAction* actionViewVPNext;   
	QAction* actionViewShowTags;

	QAction* selectNodes;
	QAction* selectEdges;
	QAction* selectFaces;
	QAction* selectElems;

	QAction*	actionColorMap;
	QAction*	actionPlay;

	CFileViewer*	fileViewer;
	CModelViewer*	modelViewer;
	CMaterialPanel*	matPanel;
	CDataPanel*		dataPanel;
	CStatePanel*	statePanel;
	CToolsPanel*	toolsPanel;
	CTimePanel*		timePanel;
	CGLView*		glview;
	QTabBar*		tab;

	CDataFieldSelector*	selectData;
	QSpinBox* pspin;
	QSpinBox*	pangle;

	QToolBar* playToolBar;
	QToolBar* pFontToolBar;

	QFontComboBox*	pFontStyle;
	QSpinBox*		pFontSize;
	QAction*		actionFontBold;
	QAction*		actionFontItalic;

	QList<CGraphWindow*>	graphList;
	CSummaryWindow*			summaryWindow;
	CIntegrateWindow*		integrateWindow;
	CStatsWindow*			statsWindow;

	QStatusBar*		statusBar;
	QProgressBar*	fileProgress;
	QToolButton*	stopFileReading;

	QString currentPath;

	QStringList	m_recentFiles;

	bool	m_update_spin;

	int		m_theme;

	bool	m_isAnimating;

	QString	m_old_title;

public:
	// build the UI
	void setupUi(CMainWindow* MainWindow)
	{
		summaryWindow = 0;
		integrateWindow = 0;
		statsWindow = 0;
		pwnd = MainWindow;

		m_theme = 0;

		m_update_spin = true;

		m_isAnimating = false;

		currentPath = QDir::currentPath();

		// set the initial window size
        MainWindow->resize(wndWidth, wndHeight);

		// create the central widget
		QWidget* centralWidget = new QWidget;
		tab = new QTabBar;
		tab->setObjectName("tab");
		tab->setExpanding(false);
		tab->setTabsClosable(true);
		glview = new CGLView(MainWindow);
		QVBoxLayout* l = new QVBoxLayout;
		l->setMargin(0);
		l->addWidget(tab);
		l->addWidget(glview);
		centralWidget->setLayout(l);

		MainWindow->setCentralWidget(centralWidget);

		// build the menu
		buildMenu(MainWindow);

		// set default tab position
		MainWindow->setTabPosition(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea, QTabWidget::North);

		// build the dockable windows
		// (must be done after menu is created)
		buildDockWidgets(MainWindow);

		// build status bar
		statusBar = new QStatusBar(MainWindow);
		MainWindow->setStatusBar(statusBar);

		fileProgress = new QProgressBar;
		fileProgress->setRange(0, 100);
		fileProgress->setMaximumWidth(200);
//		fileProgress->setMaximumHeight(15);

		stopFileReading = new QToolButton;
		stopFileReading->setAutoRaise(true);
		stopFileReading->setToolTip("<font color=\"black\">Cancel");
		stopFileReading->setIcon(QIcon(":/icons/cancel.png"));

		// connect all the slots
		QMetaObject::connectSlotsByName(MainWindow);
	}

	// build the dockable windows
	// Note that this must be called after the menu is created.
	void buildDockWidgets(CMainWindow* MainWindow)
	{
        QDockWidget* dock1 = new QDockWidget("Files", MainWindow); dock1->setObjectName("dockFiles");
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        fileViewer = new CFileViewer(MainWindow, dock1);
        fileViewer->setObjectName(QStringLiteral("fileViewer"));
        dock1->setWidget(fileViewer);
		MainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock1);
		menuView->addAction(dock1->toggleViewAction());

		QDockWidget* dock2 = new QDockWidget("Model", MainWindow); dock2->setObjectName("dockModel");
        dock2->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		modelViewer = new CModelViewer(MainWindow, dock2);
		dock2->setWidget(modelViewer);
		menuView->addAction(dock2->toggleViewAction());
		MainWindow->tabifyDockWidget(dock1, dock2);

		QDockWidget* dock3 = new QDockWidget("Materials", MainWindow); dock3->setObjectName("dockMaterials");
        dock3->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		matPanel = new CMaterialPanel(MainWindow, dock3);
		dock3->setWidget(matPanel);
		menuView->addAction(dock3->toggleViewAction());
		MainWindow->tabifyDockWidget(dock2, dock3);

		QDockWidget* dock4 = new QDockWidget("Data", MainWindow); dock4->setObjectName("dockData");
        dock4->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		dataPanel = new CDataPanel(MainWindow, dock4);
		dock4->setWidget(dataPanel);
		menuView->addAction(dock4->toggleViewAction());
		MainWindow->tabifyDockWidget(dock3, dock4);

		QDockWidget* dock5 = new QDockWidget("State", MainWindow); dock5->setObjectName("dockState");
        dock5->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		statePanel = new CStatePanel(MainWindow, dock5);
		dock5->setWidget(statePanel);
		menuView->addAction(dock5->toggleViewAction());
		MainWindow->tabifyDockWidget(dock4, dock5);

		QDockWidget* dock6 = new QDockWidget("Tools", MainWindow); dock6->setObjectName("dockTools");
        dock6->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		toolsPanel = new CToolsPanel(MainWindow, dock6);
		dock6->setWidget(toolsPanel);
		menuView->addAction(dock6->toggleViewAction());
		MainWindow->tabifyDockWidget(dock5, dock6);
		dock6->hide();

		QDockWidget* dock7 = new QDockWidget("State Selector", MainWindow); dock7->setObjectName("dockTime");
		dock7->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
		timePanel = new CTimePanel(MainWindow, dock7);
		dock7->setWidget(timePanel);
		menuView->addAction(dock7->toggleViewAction());
		MainWindow->addDockWidget(Qt::BottomDockWidgetArea, dock7);

		// make sure the file viewer is the visible tab
		dock1->raise();
	}

	QAction* addAction(const QString& title, const QString& name, const QString& iconFile = QString())
	{
		QAction* pa = new QAction(title, pwnd);
		pa->setObjectName(name);
		if (iconFile.isEmpty() == false) pa->setIcon(QIcon(iconFile));
		return pa;
	}

	// create actions and menu
	void buildMenu(CMainWindow* MainWindow)
	{
		// create actions
		// --- File menu ---
		QAction* actionOpen        = addAction("Open ..."         , "actionOpen"  , ":/icons/open.png"   ); actionOpen->setShortcuts(QKeySequence::Open);
		QAction* actionSave        = addAction("Save ..."         , "actionSave"  , ":/icons/save.png"   ); actionSave->setShortcuts(QKeySequence::Save);
		QAction* actionUpdate      = addAction("Update"           , "actionUpdate", ":/icons/refresh.png"); actionUpdate->setShortcuts(QKeySequence::Refresh);
		QAction* actionFileInfo    = addAction("File Info ..."    , "actionFileInfo");
		QAction* actionSnapShot    = addAction("Snapshot ..."     , "actionSnapShot", ":/icons/snapshot.png");
		QAction* actionOpenSession = addAction("Open session ..." , "actionOpenSession");
		QAction* actionSaveSession = addAction("Save session ..." , "actionSaveSession");
		QAction* actionQuit        = addAction("Exit"             , "actionQuit"       );

		actionOpen->setWhatsThis("<font color=\"black\">Select this to open a file.");
		actionSave->setWhatsThis("<font color=\"black\">Select this to save the model.");
		actionUpdate->setWhatsThis("<font color=\"black\">Reload the model");
		actionFileInfo->setWhatsThis("<font color=\"black\">Display file info dialog box");
		actionSnapShot->setWhatsThis("<font color=\"black\">Take a screen shot of the Graphics View");
		actionOpenSession->setWhatsThis("<font color=\"black\">Open a PostView session file");
		actionSaveSession->setWhatsThis("<font color=\"black\">Save the current PostView session");
		actionQuit->setWhatsThis("<font color=\"black\">Quit PostView");

		// --- Edit menu ---
		selectNodes = addAction("Select Nodes"   , "selectNodes", ":/icons/node.png"   ); selectNodes->setCheckable(true);
		selectEdges = addAction("Select Edges"   , "selectEdges", ":/icons/edge.png"   ); selectEdges->setCheckable(true);
		selectFaces = addAction("Select Faces"   , "selectFaces", ":/icons/face.png"   ); selectFaces->setCheckable(true);
		selectElems = addAction("Select Elements", "selectElems", ":/icons/element.png"); selectElems->setCheckable(true);

		selectNodes->setWhatsThis("<font color=\"black\">Turn on node selection mode");
		selectEdges->setWhatsThis("<font color=\"black\">Turn on edge selection mode");
		selectFaces->setWhatsThis("<font color=\"black\">Turn on face selection mode");
		selectElems->setWhatsThis("<font color=\"black\">Turn on element selection mode");
		
		QAction* actionZoomSelected = addAction("Zoom selected", "actionZoomSelected", ":/icons/zoom_selected.png"); actionZoomSelected->setShortcut(Qt::Key_S);
		QAction* actionZoomExtents  = addAction("Zoom extents" , "actionZoomExtents" , ":/icons/zoom_extents.png");

		actionZoomSelected->setWhatsThis("<font color=\"black\">Click this to zoom in on the current selection.\nIf nothing selected this will zoom to the entire model");
		actionZoomExtents->setWhatsThis("<font color=\"black\">Click this to zoom out so the whole model is visible");

		QAction* actionSelectRect   = addAction("Select Rectangle", "actionSelectRect"  , ":/icons/select_rect.png"  ); actionSelectRect->setCheckable(true);
		QAction* actionSelectCircle = addAction("Select Circle"   , "actionSelectCircle", ":/icons/select_circle.png"); actionSelectCircle->setCheckable(true);
		QAction* actionSelectFree   = addAction("Select Free"     , "actionSelectFree"  , ":/icons/select_free.png"  ); actionSelectFree->setCheckable(true);

		actionSelectRect->setWhatsThis("<font color=\"black\">Use a rectangle for rubberbanding a selection");
		actionSelectCircle->setWhatsThis("<font color=\"black\">Use a circle for rubberbanding a selection");
		actionSelectFree->setWhatsThis("<font color=\"black\">Use a free-hand curve for rubberbanding selection");

//		QAction* actionSelectItem = addAction("Select item"     , "actionSelectItem", ":/icons/select_item.png"   ); actionSelectItem->setCheckable(true);
		QAction* actionSelectConn = addAction("Select connected", "actionSelectConn", ":/icons/select_connected.png"); actionSelectConn->setCheckable(true);

//		actionSelectItem->setWhatsThis("<font color=\"black\">Clicking this option will only allow a user to select a single item (node, edge, face, element) on the mesh");
		actionSelectConn->setWhatsThis("<font color=\"black\">Clicking this option will select all items that are connected on the same partition of the last selected item");

		QAction* actionHideSelected    = addAction("Hide selected"   , "actionHideSelected"   ); actionHideSelected->setShortcut(Qt::Key_H);
		QAction* actionHideUnselected  = addAction("Hide unselected" , "actionHideUnselected" );
		QAction* actionInvertSelection = addAction("Invert selection", "actionInvertSelection");
		QAction* actionUnhideAll       = addAction("Unhide all      ", "actionUnhideAll"      ); actionUnhideAll->setShortcut(Qt::Key_U);
		QAction* actionSelectAll       = addAction("Select all      ", "actionSelectAll"      );
		QAction* actionSelectRange     = addAction("Select range ...", "actionSelectRange"    );
		QAction* actionClearSelection  = addAction("Clear selection" , "actionClearSelection" ); actionClearSelection->setShortcut(Qt::Key_Escape);
		QAction* actionFind            = addAction("Find ..."        , "actionFind"           ); actionFind->setShortcut(Qt::CTRL + Qt::Key_F);
		QAction* actionDelete          = addAction("Delete ..."      , "actionDelete"         );
		QAction* actionProperties      = addAction("Properties ..."  , "actionProperties"     , ":/icons/properties.png");

		actionHideSelected->setWhatsThis("<font color=\"black\">Click this to hide the current mesh selection");
		actionHideUnselected->setWhatsThis("<font color=\"black\">Click this to hide the mesh items that are <i>not</i> selected.");
		actionInvertSelection->setWhatsThis("<font color=\"black\">Click this to invert the selection (i.e. selected items will be unselected and vice versa");
		actionUnhideAll->setWhatsThis("<font color=\"black\">Unhide all the previously hidden items");
		actionSelectAll->setWhatsThis("<font color=\"black\">Select all items in the mesh. What items will be selected depends on the current selection mode");
		actionSelectRange->setWhatsThis("<font color=\"black\">This opens a dialog box that allows users to select items within a value range");
		actionClearSelection->setWhatsThis("<font color=\"black\">This clears the current selection");
		actionFind->setWhatsThis("<font color=\"black\">Find mesh items from a list of IDs");
		actionDelete->setWhatsThis("<font color=\"black\">Delete the selected widget in the Graphics View. Note that not all widgets can be deleted");
		actionProperties->setWhatsThis("<font color=\"black\">Opens dialog box with properties of the currently selected widget");

		// --- Post menu ---
		QAction* actionPlaneCut        = addAction("Plane cut"        , "actionPlaneCut"       , ":/icons/cut.png");
		QAction* actionVectorPlot      = addAction("Vector plot"      , "actionVectorPlot"     , ":/icons/vectors.png");
		QAction* actionTensorPlot      = addAction("Tensor plot"      , "actionTensorPlot"     , ":/icons/tensor.png");
		QAction* actionIsosurfacePlot  = addAction("Isosurface plot"  , "actionIsosurfacePlot" , ":/icons/isosurface.png");
		QAction* actionSlicePlot       = addAction("Slice plot"       , "actionSlicePlot"      , ":icons/slice.png");
		QAction* actionDisplacementMap = addAction("Displacement map" , "actionDisplacementMap", ":/icons/distort.png");
		QAction* actionStreamLinePlot  = addAction("Stream lines plot", "actionStreamLinePlot" , ":/icons/streamlines.png");
		QAction* actionParticleFlowPlot= addAction("Particle flow plot", "actionParticleFlowPlot" , ":/icons/particle.png");
		QAction* actionGraph           = addAction("New Graph ..."    , "actionGraph"          , ":/icons/chart.png"); actionGraph->setShortcut(Qt::Key_F3);
		QAction* actionSummary         = addAction("Summary ..."      , "actionSummary"        ); actionSummary->setShortcut(Qt::Key_F4);
		QAction* actionStats           = addAction("Statistics  ..."  , "actionStats"          );
		QAction* actionIntegrate       = addAction("Integrate ..."    , "actionIntegrate"      , ":/icons/integrate.png");

		actionPlaneCut->setWhatsThis("<font color=\"black\"><h3>Plane cut</h3>Add a plane cut plot to the model. A plane cut plot allows users to create a cross section of the mesh.</font>");
		actionVectorPlot->setWhatsThis("<font color=\"black\"><h3>Vector plot</h3>Add a vector plot to the model. Vectors plots can show vector data in the model");
		actionTensorPlot->setWhatsThis("<font color=\"black\"><h3>Tensor plot</h3>Add a tensor plot to the model. Tensor plots can show 2nd order tensor data in the model");
		actionIsosurfacePlot->setWhatsThis("<font color=\"black\"><h3>Iso-surface plot</h3>Add an iso-surface plot to the model. An iso-surface plot shows surfaces that have the same value. You may need to make the model transparent in order to see the iso surfaces.");
		actionSlicePlot->setWhatsThis("<font color=\"black\"><h3>Slice plot</h3>Add a slice plot. This plot adds several cross sections to the model. You may need to make the model transparent to see the slices.");
		actionDisplacementMap->setWhatsThis("<font color=\"black\"><h3>Displacement map</h3>Adds a displacement map. A displacement map will deform the model as a function of time.");
		actionGraph->setWhatsThis("<font color=\"black\">Create a new Graph window");
		actionSummary->setWhatsThis("<font color=\"black\">Shows the Summary window.The Summary window shows the min, max, and average values of a user-selected data field");
		actionStats->setWhatsThis("<font color=\"black\">Shows the Statistics window. This window shows the distribution of the current nodal values at the current time step");
		actionIntegrate->setWhatsThis("<font color=\"black\">Shows a graph that plots the integral of the values of the current selection as a function of time. Note that for a surface select it calculates a surface integral and for an element section, it shows a volume integral. For a node selection, the nodal values are summed.");

		// --- Record menu ---
		QAction* actionRecordNew   = addAction("New ...", "actionRecordNew"  );
		QAction* actionRecordStart = addAction("Start"  , "actionRecordStart");
		QAction* actionRecordPause = addAction("Pause"  , "actionRecordPause");
		QAction* actionRecordStop  = addAction("Stop"   , "actionRecordStop" );

		actionRecordNew->setWhatsThis("<font color=\"black\">Click this to open a file dialog box and create a new animation file.");
		actionRecordStart->setWhatsThis("<font color=\"black\">Click to start recording an animation. You must create an animation file first before you can start recording.");
		actionRecordPause->setWhatsThis("<font color=\"black\">Click this pause the current recording");
		actionRecordStop->setWhatsThis("<font color=\"black\">Click this to stop the recording. This will finalize and close the animation file as well.");

		// --- View Menu ---
		actionViewSettings   = addAction("Settings ..."           , "actionViewSettings"  );
		actionViewCapture    = addAction("Show capture Frame"     , "actionViewCapture"   ); actionViewCapture->setCheckable(true); actionViewCapture->setShortcut(Qt::Key_0);
		actionViewWidgets    = addAction("Show widgets"           , "actionViewWidgets"   ); actionViewWidgets->setCheckable(true); actionViewWidgets->setChecked(true);
		actionViewProjection = addAction("Orthographic projection", "actionViewProjection"); actionViewProjection->setShortcut(Qt::CTRL + Qt::Key_0); actionViewProjection->setCheckable(true);
		actionViewMesh       = addAction("Show mesh lines"        , "actionViewMesh"      ); actionViewMesh   ->setShortcut(Qt::Key_M); actionViewMesh->setCheckable(true);
		actionViewOutline    = addAction("Show outline"           , "actionViewOutline"   ); actionViewOutline->setShortcut(Qt::Key_Z); actionViewOutline->setCheckable(true);
		actionViewShowTags   = addAction("Show tags"              , "actionViewShowTags"  ); actionViewShowTags->setShortcut(Qt::Key_T); actionViewShowTags->setCheckable(true); actionViewShowTags->setChecked(true);
		actionViewSmooth     = addAction("Color smoothing"        , "actionViewSmooth"    ); actionViewSmooth ->setShortcut(Qt::Key_C); actionViewSmooth->setCheckable(true);
		actionViewFront      = addAction("Front",                  "actionViewFront"     ); actionViewFront->setShortcut(Qt::CTRL + Qt::Key_1);
		actionViewBack       = addAction("Back" ,                  "actionViewBack"      ); actionViewBack->setShortcut(Qt::CTRL + Qt::Key_3);
		actionViewLeft       = addAction("Left" ,                  "actionViewLeft"      ); actionViewLeft->setShortcut(Qt::CTRL + Qt::Key_4);
		actionViewRight      = addAction("Right",                  "actionViewRight"     ); actionViewRight->setShortcut(Qt::CTRL + Qt::Key_6);
		actionViewTop        = addAction("Top" ,                   "actionViewTop"       ); actionViewTop->setShortcut(Qt::CTRL + Qt::Key_8);
		actionViewBottom     = addAction("Bottom" ,                "actionViewBottom"    ); actionViewBottom->setShortcut(Qt::CTRL + Qt::Key_2);
		actionViewTrack      = addAction("Track selection",        "actionViewTrack"     ); actionViewTrack->setShortcut(Qt::CTRL + Qt::Key_T); actionViewTrack->setCheckable(true);
		actionViewVPSave     = addAction("Save viewpoint",         "actionViewVPSave"    ); actionViewVPSave->setShortcut(Qt::CTRL + Qt::Key_K);
		actionViewVPPrev     = addAction("Prev viewpoint",         "actionViewVPPrev"    ); actionViewVPPrev->setShortcut(Qt::CTRL + Qt::Key_J);
		actionViewVPNext     = addAction("Next viewpoint",         "actionViewVPNext"    ); actionViewVPNext->setShortcut(Qt::CTRL + Qt::Key_L);

		QAction* whatsThis = QWhatsThis::createAction(MainWindow);
		whatsThis->setWhatsThis("<font color=\"black\">Click this to enter What's This mode. When selected any ui item can be selected and a brief description of the feature is shown");


		// --- Help Menu ---
		QAction* actionHelp  = new QAction("Online Help ... " , MainWindow); actionHelp ->setObjectName(QStringLiteral("actionHelp" ));
		QAction* actionAbout = new QAction("About ... ", MainWindow); actionAbout->setObjectName(QStringLiteral("actionAbout"));

		// create menu bar
		QMenuBar* menuBar = MainWindow->menuBar();
		menuFile   = new QMenu("File"  , menuBar);
		menuEdit   = new QMenu("Edit"  , menuBar);
		menuPost   = new QMenu("Post"  , menuBar);
		menuRecord = new QMenu("Record", menuBar);
		menuView   = new QMenu("View"  , menuBar);
		menuHelp   = new QMenu("Help"  , menuBar);

		menuRecentFiles = new QMenu("Recent Files");

		recentFilesActionGroup = new QActionGroup(MainWindow);
		recentFilesActionGroup->setObjectName("recentFiles");

		QActionGroup* pag = new QActionGroup(MainWindow);
		pag->addAction(selectNodes);
		pag->addAction(selectEdges);
		pag->addAction(selectFaces);
		pag->addAction(selectElems);
		selectElems->setChecked(true);

		pag = new QActionGroup(MainWindow);
		pag->addAction(actionSelectRect);
		pag->addAction(actionSelectCircle);
		pag->addAction(actionSelectFree);
		actionSelectRect->setChecked(true);

//		pag = new QActionGroup(MainWindow);
	//	pag->addAction(actionSelectItem);
//		pag->addAction(actionSelectConn);
//		actionSelectItem->setChecked(true);

		// build the menu
		menuBar->addAction(menuFile->menuAction());
		menuFile->addAction(actionOpen);
		menuFile->addAction(actionSave); 
		menuFile->addAction(actionUpdate);
		menuFile->addAction(actionFileInfo);
		menuFile->addAction(menuRecentFiles->menuAction());
		menuFile->addSeparator();
		menuFile->addAction(actionSnapShot);
		menuFile->addSeparator();
		menuFile->addAction(actionOpenSession);
		menuFile->addAction(actionSaveSession);
		menuFile->addSeparator();
		menuFile->addAction(actionQuit);

		menuBar->addAction(menuEdit->menuAction());
		menuEdit->addAction(selectNodes);
		menuEdit->addAction(selectEdges);
		menuEdit->addAction(selectFaces);
		menuEdit->addAction(selectElems);
		menuEdit->addSeparator();
		menuEdit->addAction(actionHideSelected);
		menuEdit->addAction(actionHideUnselected);
		menuEdit->addAction(actionInvertSelection);
		menuEdit->addAction(actionUnhideAll);
		menuEdit->addAction(actionSelectAll);
		menuEdit->addAction(actionSelectRange);
		menuEdit->addAction(actionClearSelection);
		menuEdit->addSeparator();
		menuEdit->addAction(actionFind);
		menuEdit->addSeparator();
		menuEdit->addAction(actionDelete);
		menuEdit->addAction(actionProperties);

		menuBar->addAction(menuPost->menuAction());
		menuPost->addAction(actionPlaneCut);
		menuPost->addAction(actionVectorPlot);
		menuPost->addAction(actionTensorPlot);
		menuPost->addAction(actionIsosurfacePlot);
		menuPost->addAction(actionSlicePlot);
		menuPost->addAction(actionDisplacementMap);
		menuPost->addAction(actionStreamLinePlot);
		menuPost->addAction(actionParticleFlowPlot);
		menuPost->addSeparator();
		menuPost->addAction(actionGraph);
		menuPost->addSeparator();
		menuPost->addAction(actionSummary);
		menuPost->addAction(actionStats);
		menuPost->addAction(actionIntegrate);

		menuBar->addAction(menuRecord->menuAction());
		menuRecord->addAction(actionRecordNew);
		menuRecord->addSeparator();
		menuRecord->addAction(actionRecordStart);
		menuRecord->addAction(actionRecordPause);
		menuRecord->addAction(actionRecordStop);

		menuBar->addAction(menuView->menuAction());
		menuView->addAction(actionViewSettings  );
		menuView->addSeparator();
		menuView->addAction(actionViewCapture   );
		menuView->addAction(actionViewProjection);
		menuView->addAction(actionViewWidgets   );
		menuView->addAction(actionViewMesh      );
		menuView->addAction(actionViewShowTags  );
		menuView->addAction(actionViewOutline   );
		menuView->addAction(actionViewSmooth    );
		menuView->addSeparator();
		menuView->addAction(actionViewFront     );
		menuView->addAction(actionViewBack      );
		menuView->addAction(actionViewLeft      );
		menuView->addAction(actionViewRight     );
		menuView->addAction(actionViewTop       );
		menuView->addAction(actionViewBottom    );
		menuView->addSeparator();
		menuView->addAction(actionViewTrack     );
		menuView->addSeparator();
		menuView->addAction(actionViewVPSave    );
		menuView->addAction(actionViewVPPrev    );
		menuView->addAction(actionViewVPNext    );
		menuView->addSeparator();

		menuBar->addAction(menuHelp->menuAction());
		menuHelp->addAction(actionHelp);
		menuHelp->addAction(actionAbout);

		// Create the toolbar
		QToolBar* mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

		actionColorMap = addAction("Toggle colormap" , "actionColorMap"       , ":/icons/colormap.png");
		actionColorMap->setCheckable(true);
		actionColorMap->setDisabled(true);
		actionColorMap->setWhatsThis("<font color=\"black\">Click this to turn on the color map on the model.");

		mainToolBar->addAction(actionOpen);
		mainToolBar->addAction(actionSave);
		mainToolBar->addAction(actionUpdate);
		mainToolBar->addAction(actionSnapShot);
		mainToolBar->addSeparator();

		mainToolBar->addAction(selectNodes);
		mainToolBar->addAction(selectEdges);
		mainToolBar->addAction(selectFaces);
		mainToolBar->addAction(selectElems);
		mainToolBar->addSeparator();

//		mainToolBar->addAction(actionSelectItem);
		mainToolBar->addAction(actionSelectConn);
		mainToolBar->addWidget(pangle = new QSpinBox);
		pangle->setObjectName("selectAngle");
		pangle->setMinimumWidth(80);
		pangle->setRange(0, 180);

		mainToolBar->addAction(actionSelectRect  );
		mainToolBar->addAction(actionSelectCircle);
		mainToolBar->addAction(actionSelectFree  );

		mainToolBar->addSeparator();
		mainToolBar->addAction(actionZoomSelected);
		mainToolBar->addAction(actionZoomExtents);

		// create the data field selector
		mainToolBar->addSeparator();
		selectData = new CDataFieldSelector;
		selectData->setWhatsThis("<font color=\"black\">Use this to select the current data variable that will be used to display the color map on the mesh.");
		selectData->setMinimumWidth(300);
//		selectData->setFixedHeight(23);
		selectData->setObjectName("selectData");

		mainToolBar->addWidget(selectData);
		mainToolBar->addAction(actionColorMap);

		mainToolBar->addAction(whatsThis);

		// --- Post Tool bar
		QToolBar* postToolbar = new QToolBar(MainWindow);
		postToolbar->setObjectName("postToolbar");
		postToolbar->addAction(actionPlaneCut      );
		postToolbar->addAction(actionVectorPlot    );
		postToolbar->addAction(actionTensorPlot);
		postToolbar->addAction(actionIsosurfacePlot);
		postToolbar->addAction(actionSlicePlot     );
		postToolbar->addAction(actionStreamLinePlot);
		postToolbar->addAction(actionParticleFlowPlot);
		postToolbar->addSeparator();
		postToolbar->addAction(actionGraph);
		postToolbar->addAction(actionIntegrate);
		MainWindow->addToolBar(Qt::TopToolBarArea, postToolbar);

		// Play tool bar
		playToolBar = new QToolBar(MainWindow);
        playToolBar->setObjectName(QStringLiteral("playToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, playToolBar);

		QAction* actionFirst = addAction("first", "actionFirst", ":/icons/back.png");
		QAction* actionPrev  = addAction("previous", "actionPrev", ":/icons/prev.png");
		actionPlay = addAction("Play", "actionPlay", ":/icons/play.png"); actionPlay->setShortcut(Qt::Key_Space);
		actionPlay->setCheckable(true);
		QAction* actionNext = addAction("next", "actionNext", ":/icons/next.png");
		QAction* actionLast = addAction("last", "actionLast", ":/icons/forward.png");
		QAction* actionTime = addAction("Time settings", "actionTimeSettings", ":/icons/clock.png");

		playToolBar->addAction(actionFirst);
		playToolBar->addAction(actionPrev);
		playToolBar->addAction(actionPlay);
		playToolBar->addAction(actionNext);
		playToolBar->addAction(actionLast);

		actionFirst->setWhatsThis("<font color=\"black\">Click this to go to the first time step in the model.");
		actionPrev->setWhatsThis("<font color=\"black\">Click this to go to the previous time step in the model.");
		actionPlay->setWhatsThis("<font color=\"black\">Click this to toggle the animation on or off");
		actionNext->setWhatsThis("<font color=\"black\">Click this to go to the next time step");
		actionLast->setWhatsThis("<font color=\"black\">Click this to go to the last time step in the model.");
		actionTime->setWhatsThis("<font color=\"black\">Click this to open the Time Info dialog box.");

		playToolBar->addWidget(pspin = new QSpinBox); 
		pspin->setObjectName("selectTime");
		pspin->setMinimumWidth(80);
		pspin->setSuffix("/100");
		playToolBar->addAction(actionTime);
	
		playToolBar->setDisabled(true);

		// Font tool bar
		pFontToolBar = new QToolBar(MainWindow);
		pFontToolBar->setObjectName("FontToolBar");
		MainWindow->addToolBarBreak();
		MainWindow->addToolBar(Qt::TopToolBarArea, pFontToolBar);

		pFontToolBar->addWidget(pFontStyle = new QFontComboBox); pFontStyle->setObjectName("fontStyle");
		pFontToolBar->addWidget(pFontSize = new QSpinBox); pFontSize->setObjectName("fontSize");
		pFontToolBar->addAction(actionFontBold   = addAction("Bold"  , "fontBold"  , ":/icons/font_bold.png"  )); actionFontBold->setCheckable(true);
		pFontToolBar->addAction(actionFontItalic = addAction("Italic", "fontItalic", ":/icons/font_italic.png")); actionFontItalic->setCheckable(true);
		pFontToolBar->addAction(actionProperties);
		pFontToolBar->setEnabled(false);
	}

	void checkColormap(bool b)
	{
		actionColorMap->setChecked(b);
	}

	void setRecentFiles(QStringList& recentFiles)
	{
		m_recentFiles = recentFiles;

		int N = m_recentFiles.count();
		if (N > MAX_RECENT_FILES) N = MAX_RECENT_FILES;

		for (int i = 0; i < N; ++i)
		{
			QString file = m_recentFiles.at(i);

			QAction* pa = menuRecentFiles->addAction(file);

			recentFilesActionGroup->addAction(pa);
		}
	}

	void addToRecentFiles(const QString& file)
	{
		// see if the file already exists or not
		if (m_recentFiles.contains(file)) return;

		// add a new file item
		m_recentFiles.append(file);
		QAction* pa = menuRecentFiles->addAction(file);
		recentFilesActionGroup->addAction(pa);

		int N = m_recentFiles.count();
		if (N > MAX_RECENT_FILES)
		{
			// remove the first one
			m_recentFiles.removeAt(0);
			QAction* pa = menuRecentFiles->actions().first();
			menuRecentFiles->removeAction(pa);
		}
	}

	void addTab(const QString& tabTitle, const QString& toolTip)
	{
		tab->addTab(tabTitle);
		tab->setCurrentIndex(tab->count() - 1);
		tab->setTabToolTip(tab->count() - 1, toolTip);
	}

	void RemoveTab(int i)
	{
		tab->removeTab(i);
	}
};

namespace Ui {
    class CMainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE
