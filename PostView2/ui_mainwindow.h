#pragma once
#include <QHBoxLayout>
#include <QMenuBar>
#include <QTextEdit>
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>
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
#include <QFontComboBox>
#include <QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
	enum 
	{
		wndWidth   = 800,		// width of main window
		wndHeight  = 600,		// height of main window
		menuHeight = 21			// height of menu bar
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

	QAction* actionViewSettings;
	QAction* actionViewCapture; 
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

	QAction*	actionColorMap;
	QAction*	actionPlay;

	CFileViewer*	fileViewer;
	CModelViewer*	modelViewer;
	CMaterialPanel*	matPanel;
	CDataPanel*		dataPanel;
	CStatePanel*	statePanel;
	CToolsPanel*	toolsPanel;
	CGLView*		glview;

	CDataFieldSelector*	selectData;
	QSpinBox* pspin;

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

public:
	// build the UI
	void setupUi(CMainWindow* MainWindow)
	{
		summaryWindow = 0;
		integrateWindow = 0;
		statsWindow = 0;
		pwnd = MainWindow;

		// set the initial window size
        MainWindow->resize(wndWidth, wndHeight);

		// create the central widget
		glview = new CGLView(MainWindow);

		MainWindow->setCentralWidget(glview);

		// build the menu
		buildMenu(MainWindow);

		// build the dockable windows
		// (must be done after menu is created)
		buildDockWidgets(MainWindow);

		// build status bar
		QStatusBar* statusBar = new QStatusBar(MainWindow);
		MainWindow->setStatusBar(statusBar);

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
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
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
		MainWindow->addDockWidget(Qt::RightDockWidgetArea, dock6);
		dock6->hide();

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
		QAction* actionSnapShot    = addAction("Snapshot ..."     , "actionSnapShot", ":/icons/snapshot.png");
		QAction* actionOpenSession = addAction("Open session ..." , "actionOpenSession");
		QAction* actionSaveSession = addAction("Save session ..." , "actionSaveSession");
		QAction* actionQuit        = addAction("Exit"             , "actionQuit"       );

		// --- Edit menu ---
		QAction* selectNodes = addAction("Select Nodes"   , "selectNodes", ":/icons/node.png"   ); selectNodes->setCheckable(true);
		QAction* selectEdges = addAction("Select Edges"   , "selectEdges", ":/icons/edge.png"   ); selectEdges->setCheckable(true);
		QAction* selectFaces = addAction("Select Faces"   , "selectFaces", ":/icons/face.png"   ); selectFaces->setCheckable(true);
		QAction* selectElems = addAction("Select Elements", "selectElems", ":/icons/element.png"); selectElems->setCheckable(true);

		QAction* actionHideSelected    = addAction("Hide selected"   , "actionHideSelected"   );
		QAction* actionHideUnselected  = addAction("Hide unselected" , "actionHideUnselected" );
		QAction* actionInvertSelection = addAction("Invert selection", "actionInvertSelection");
		QAction* actionUnhideAll       = addAction("Unhide all      ", "actionUnhideAll"      );
		QAction* actionSelectAll       = addAction("Select all      ", "actionSelectAll"      );
		QAction* actionSelectRange     = addAction("Select range ...", "actionSelectRange"    );
		QAction* actionClearSelection  = addAction("Clear selection" , "actionClearSelection" ); actionClearSelection->setShortcut(Qt::Key_Escape);
		QAction* actionFind            = addAction("Find ..."        , "actionFind"           ); actionFind->setShortcut(Qt::CTRL + Qt::Key_F);
		QAction* actionDelete          = addAction("Delete ..."      , "actionDelete"         );
		QAction* actionProperties      = addAction("Properties ..."  , "actionProperties"     , ":/icons/properties.png");

		// --- Post menu ---
		QAction* actionPlaneCut        = addAction("Plane cut"       , "actionPlaneCut"       , ":/icons/cut.png");
		QAction* actionVectorPlot      = addAction("Vector plot"     , "actionVectorPlot"     , ":/icons/vectors.png");
		QAction* actionIsosurfacePlot  = addAction("Isosurface plot" , "actionIsosurfacePlot" , ":/icons/isosurface.png");
		QAction* actionSlicePlot       = addAction("Slice plot"      , "actionSlicePlot"      , ":icons/slice.png");
		QAction* actionDisplacementMap = addAction("Displacement map", "actionDisplacementMap", ":/icons/distort.png");
		QAction* actionGraph           = addAction("New Graph ..."   , "actionGraph"          , ":/icons/chart.png"); actionGraph->setShortcut(Qt::Key_F3);
		QAction* actionSummary         = addAction("Summary ..."     , "actionSummary"        );
		QAction* actionStats           = addAction("Statistics  ..." , "actionStats"          );
		QAction* actionIntegrate       = addAction("Integrate ..."   , "actionIntegrate"      , ":/icons/integrate.png");

		// --- Record menu ---
		QAction* actionRecordNew   = addAction("New ...", "actionRecordNew"  );
		QAction* actionRecordStart = addAction("Start"  , "actionRecordStart");
		QAction* actionRecordPause = addAction("Pause"  , "actionRecordPause");
		QAction* actionRecordStop  = addAction("Stop"   , "actionRecordStop" );

		// --- View Menu ---
		actionViewSettings   = addAction("Settings ..."           , "actionViewSettings"  );
		actionViewCapture    = addAction("Show capture Frame"     , "actionViewCapture"   ); actionViewCapture->setCheckable(true);
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

		// --- Help Menu ---
		QAction* actionHelp  = new QAction("Help ... " , MainWindow); actionHelp ->setObjectName(QStringLiteral("actionHelp" ));
		QAction* actionAbout = new QAction("About ... ", MainWindow); actionAbout->setObjectName(QStringLiteral("actionAbout"));

		// create menu bar
		QMenuBar* menuBar = new QMenuBar(MainWindow);
		menuBar->setGeometry(QRect(0, 0, wndWidth, menuHeight));
		menuFile   = new QMenu("File"  , menuBar);
		menuEdit   = new QMenu("Edit"  , menuBar);
		menuPost   = new QMenu("Post"  , menuBar);
		menuRecord = new QMenu("Record", menuBar);
		menuView   = new QMenu("View"  , menuBar);
		menuHelp   = new QMenu("Help"  , menuBar);
		MainWindow->setMenuBar(menuBar);

		QActionGroup* pag = new QActionGroup(MainWindow);
		pag->addAction(selectNodes);
		pag->addAction(selectEdges);
		pag->addAction(selectFaces);
		pag->addAction(selectElems);
		selectElems->setChecked(true);

		// build the menu
		menuBar->addAction(menuFile->menuAction());
		menuFile->addAction(actionOpen);
		menuFile->addAction(actionSave); 
		menuFile->addAction(actionUpdate);
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
		menuPost->addAction(actionIsosurfacePlot);
		menuPost->addAction(actionSlicePlot);
		menuPost->addAction(actionDisplacementMap);
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

		// create the data field selector
		selectData = new CDataFieldSelector;
		selectData->setMinimumWidth(200);
		selectData->setFixedHeight(23);
		selectData->setObjectName("selectData");

		mainToolBar->addWidget(selectData);
		mainToolBar->addAction(actionColorMap);

		// --- Post Tool bar
		QToolBar* postToolbar = new QToolBar(MainWindow);
		postToolbar->setObjectName("postToolbar");
		postToolbar->addAction(actionPlaneCut      );
		postToolbar->addAction(actionVectorPlot    );
		postToolbar->addAction(actionIsosurfacePlot);
		postToolbar->addAction(actionSlicePlot     );
		postToolbar->addSeparator();
		postToolbar->addAction(actionGraph);
		postToolbar->addAction(actionIntegrate);
		MainWindow->addToolBar(Qt::TopToolBarArea, postToolbar);

		// Play tool bar
		playToolBar = new QToolBar(MainWindow);
        playToolBar->setObjectName(QStringLiteral("playToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, playToolBar);

		playToolBar->addAction(addAction("first"   , "actionFirst", ":/icons/back.png"));
		playToolBar->addAction(addAction("previous", "actionPrev" , ":/icons/prev.png"));

		actionPlay = addAction("Play" , "actionPlay"       , ":/icons/play.png"); actionPlay->setShortcut(Qt::Key_Space);
		actionPlay->setCheckable(true);
		playToolBar->addAction(actionPlay);

		playToolBar->addAction(addAction("next"   , "actionNext", ":/icons/next.png"));
		playToolBar->addAction(addAction("last"   , "actionLast", ":/icons/forward.png"));

		playToolBar->addWidget(pspin = new QSpinBox); 
		pspin->setObjectName("selectTime");
		pspin->setMinimumWidth(80);
		pspin->setSuffix("/100");
		playToolBar->addAction(addAction("Time settings", "actionTimeSettings", ":/icons/clock.png"));
	
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
};

namespace Ui {
    class CMainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE
