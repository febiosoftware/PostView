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
#include "ToolsPanel.h"
#include "GLView.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
	enum 
	{
		wndWidth   = 800,		// width of main window
		wndHeight  = 600,		// height of main window
		menuHeight = 21			// height of menu bar
	};

public:
	QMenu* menuFile;
	QMenu* menuEdit; 
	QMenu* menuPost; 
	QMenu* menuRecord;
	QMenu* menuView;
	QMenu* menuHelp; 

	CFileViewer*	m_fileViewer;
	CModelViewer*	m_modelViewer;
	CMaterialPanel*	m_matPanel;
	CDataPanel*		m_dataPanel;
	CStatePanel*	m_statePanel;
	CToolsPanel*	m_toolsPanel;
	CGLView*		m_view;

public:
	// build the UI
	void setupUi(CMainWindow* MainWindow)
	{
		// set the initial window size
        MainWindow->resize(wndWidth, wndHeight);

		// create the central widget
		m_view = new CGLView(MainWindow);
		MainWindow->setCentralWidget(m_view);

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
        QDockWidget* dock1 = new QDockWidget("File Viewer", MainWindow);
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        m_fileViewer = new CFileViewer(MainWindow, dock1);
        m_fileViewer->setObjectName(QStringLiteral("fileViewer"));
        dock1->setWidget(m_fileViewer);
		MainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock1);
		menuView->addAction(dock1->toggleViewAction());

		QDockWidget* dock2 = new QDockWidget("Model Viewer", MainWindow);
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		m_modelViewer = new CModelViewer(dock2);
		dock2->setWidget(m_modelViewer);
		menuView->addAction(dock2->toggleViewAction());
		MainWindow->tabifyDockWidget(dock1, dock2);

		QDockWidget* dock3 = new QDockWidget("Materials", MainWindow);
        dock3->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		m_matPanel = new CMaterialPanel(dock3);
		dock3->setWidget(m_matPanel);
		menuView->addAction(dock3->toggleViewAction());
		MainWindow->tabifyDockWidget(dock2, dock3);

		QDockWidget* dock4 = new QDockWidget("Data", MainWindow);
        dock4->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		m_dataPanel = new CDataPanel(dock4);
		dock4->setWidget(m_dataPanel);
		menuView->addAction(dock4->toggleViewAction());
		MainWindow->tabifyDockWidget(dock3, dock4);

		QDockWidget* dock5 = new QDockWidget("State", MainWindow);
        dock5->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		m_statePanel = new CStatePanel(dock5);
		dock5->setWidget(m_statePanel);
		menuView->addAction(dock5->toggleViewAction());
		MainWindow->tabifyDockWidget(dock4, dock5);

		QDockWidget* dock6 = new QDockWidget("Tools", MainWindow);
        dock6->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		m_toolsPanel = new CToolsPanel(dock6);
		dock6->setWidget(m_toolsPanel);
		menuView->addAction(dock6->toggleViewAction());
		MainWindow->addDockWidget(Qt::RightDockWidgetArea, dock6);
		dock6->hide();

		// make sure the file viewer is the visible tab
		dock1->raise();
	}

	// create actions and menu
	void buildMenu(CMainWindow* MainWindow)
	{
		// create actions
		// --- File menu ---
		QAction* actionOpen        = new QAction("Open ..."         , MainWindow); actionOpen       ->setObjectName(QStringLiteral("actionOpen"       ));
		QAction* actionSave        = new QAction("Save ..."         , MainWindow); actionSave       ->setObjectName(QStringLiteral("actionSave"       ));
		QAction* actionUpdate      = new QAction("Update"           , MainWindow); actionUpdate     ->setObjectName(QStringLiteral("actionUpdate"     ));
		QAction* actionExport      = new QAction("Export ..."       , MainWindow); actionExport     ->setObjectName(QStringLiteral("actionExport"     ));
		QAction* actionModelInfo   = new QAction("Model Info ..."   , MainWindow); actionModelInfo  ->setObjectName(QStringLiteral("actionModelInfo"  ));
		QAction* actionSnapShot    = new QAction("Snapshot ..."     , MainWindow); actionSnapShot   ->setObjectName(QStringLiteral("actionSnapShot"   ));
		QAction* actionOpenSession = new QAction("Open session ..." , MainWindow); actionOpenSession->setObjectName(QStringLiteral("actionOpenSession"));
		QAction* actionSaveSession = new QAction("Save session ..." , MainWindow); actionSaveSession->setObjectName(QStringLiteral("actionSaveSession"));
		QAction* actionQuit        = new QAction("Exit"             , MainWindow); actionQuit       ->setObjectName(QStringLiteral("actionQuit"       ));

		// apply icons
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/document-open-8.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon);
        icon.addFile(QStringLiteral(":/icons/document-save-5.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon);
        icon.addFile(QStringLiteral(":/icons/view-refresh-3.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionUpdate->setIcon(icon);
        icon.addFile(QStringLiteral(":/icons/App-snapshot-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSnapShot->setIcon(icon);

		// --- Edit menu ---
		QAction* actionHideSelected    = new QAction("Hide selected"   , MainWindow); actionHideSelected   ->setObjectName(QStringLiteral("actionHideSelected"   ));
		QAction* actionHideUnselected  = new QAction("Hide unselected" , MainWindow); actionHideUnselected ->setObjectName(QStringLiteral("actionHideUnselected" ));
		QAction* actionInvertSelection = new QAction("Invert selection", MainWindow); actionInvertSelection->setObjectName(QStringLiteral("actionInvertSelection"));
		QAction* actionUnhideAll       = new QAction("Unhide all      ", MainWindow); actionUnhideAll      ->setObjectName(QStringLiteral("actionUnhideAll"      ));
		QAction* actionSelectAll       = new QAction("Select all      ", MainWindow); actionSelectAll      ->setObjectName(QStringLiteral("actionSelectAll"      ));
		QAction* actionSelectRange     = new QAction("Select range ...", MainWindow); actionSelectRange    ->setObjectName(QStringLiteral("actionSelectRange"    ));
		QAction* actionClearSelection  = new QAction("Clear selection" , MainWindow); actionClearSelection ->setObjectName(QStringLiteral("actionClearSelection" ));
		QAction* actionFind            = new QAction("Find ..."        , MainWindow); actionFind           ->setObjectName(QStringLiteral("actionFind"           ));
		QAction* actionDelete          = new QAction("Delete ..."      , MainWindow); actionDelete         ->setObjectName(QStringLiteral("actionDelete"         ));
		QAction* actionProperties      = new QAction("Properties ..."  , MainWindow); actionProperties     ->setObjectName(QStringLiteral("actionProperties"     ));

		// --- Post menu ---
		QAction* actionPlaneCut        = new QAction("Plane cut"       , MainWindow); actionPlaneCut       ->setObjectName(QStringLiteral("actionPlaneCut"       ));
		QAction* actionVectorPlot      = new QAction("Vector plot"     , MainWindow); actionVectorPlot     ->setObjectName(QStringLiteral("actionVectorPlot"     ));
		QAction* actionIsosurfacePlot  = new QAction("Isosurface plot" , MainWindow); actionIsosurfacePlot ->setObjectName(QStringLiteral("actionIsosurfacePlot" ));
		QAction* actionSlicePlot       = new QAction("Slice plot"      , MainWindow); actionSlicePlot      ->setObjectName(QStringLiteral("actionSlicePlot"      ));
		QAction* actionDisplacementMap = new QAction("Displacement map", MainWindow); actionDisplacementMap->setObjectName(QStringLiteral("actionDisplacementMap"));
		QAction* actionSummary         = new QAction("Summary ..."     , MainWindow); actionSummary        ->setObjectName(QStringLiteral("actionSummary"        ));
		QAction* actionStats           = new QAction("Statistics  ..." , MainWindow); actionStats          ->setObjectName(QStringLiteral("actionStats"          ));
		QAction* actionGraph           = new QAction("Graph ..."       , MainWindow); actionGraph          ->setObjectName(QStringLiteral("actionGraph"          ));
		QAction* actionIntegrate       = new QAction("Integrate ..."   , MainWindow); actionIntegrate      ->setObjectName(QStringLiteral("actionIntegrate"      ));

		// --- Record menu ---
		QAction* actionRecordNew   = new QAction("New ...", MainWindow); actionRecordNew  ->setObjectName(QStringLiteral("actionRecordNew"  ));
		QAction* actionRecordStart = new QAction("Start"  , MainWindow); actionRecordStart->setObjectName(QStringLiteral("actionRecordStart"));
		QAction* actionRecordPause = new QAction("Pause"  , MainWindow); actionRecordPause->setObjectName(QStringLiteral("actionRecordPause"));
		QAction* actionRecordStop  = new QAction("Stop"   , MainWindow); actionRecordStop ->setObjectName(QStringLiteral("actionRecordStop" ));

		// --- View Menu ---
		QAction* actionViewSettings   = new QAction("Settings ..."          , MainWindow); actionViewSettings  ->setObjectName(QStringLiteral("actionViewSettings"  ));
		QAction* actionViewCapture    = new QAction("Toggle capture Frame"  , MainWindow); actionViewCapture   ->setObjectName(QStringLiteral("actionViewCapture"   ));
		QAction* actionViewProjection = new QAction("Toggle projection mode", MainWindow); actionViewProjection->setObjectName(QStringLiteral("actionViewProjection"));
		QAction* actionViewMesh       = new QAction("Toggle mesh lines",      MainWindow); actionViewMesh      ->setObjectName(QStringLiteral("actionViewMesh"      ));
		QAction* actionViewOutline    = new QAction("Toggle outline",         MainWindow); actionViewOutline   ->setObjectName(QStringLiteral("actionViewOutline"   ));
		QAction* actionViewSmooth     = new QAction("Toggle color smoothing", MainWindow); actionViewSmooth    ->setObjectName(QStringLiteral("actionViewSmooth"    ));
		QAction* actionViewFront      = new QAction("Front",                  MainWindow); actionViewFront     ->setObjectName(QStringLiteral("actionViewFront"     ));
		QAction* actionViewBack       = new QAction("Back" ,                  MainWindow); actionViewBack      ->setObjectName(QStringLiteral("actionViewBack"      ));
		QAction* actionViewLeft       = new QAction("Left" ,                  MainWindow); actionViewLeft      ->setObjectName(QStringLiteral("actionViewLeft"      ));
		QAction* actionViewRight      = new QAction("Right",                  MainWindow); actionViewRight     ->setObjectName(QStringLiteral("actionViewRight"     ));
		QAction* actionViewTop        = new QAction("Top" ,                   MainWindow); actionViewTop       ->setObjectName(QStringLiteral("actionViewTop"       ));
		QAction* actionViewBottom     = new QAction("Bottom" ,                MainWindow); actionViewBottom    ->setObjectName(QStringLiteral("actionViewBottom"    ));
		QAction* actionViewTrack      = new QAction("Track selection",        MainWindow); actionViewTrack     ->setObjectName(QStringLiteral("actionViewTrack"     ));
		QAction* actionViewVPSave     = new QAction("Save viewpoint",         MainWindow); actionViewVPSave    ->setObjectName(QStringLiteral("actionViewVPSave"    ));
		QAction* actionViewVPPrev     = new QAction("Prev viewpoint",         MainWindow); actionViewVPPrev    ->setObjectName(QStringLiteral("actionViewVPPrev"    ));
		QAction* actionViewVPNext     = new QAction("Next viewpoint",         MainWindow); actionViewVPNext    ->setObjectName(QStringLiteral("actionViewVPNext"    ));

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

		// build the menu
		menuBar->addAction(menuFile->menuAction());
		menuFile->addAction(actionOpen);
		menuFile->addAction(actionSave); 
		menuFile->addAction(actionUpdate);
		menuFile->addSeparator();
		menuFile->addAction(actionExport);
		menuFile->addSeparator();
		menuFile->addAction(actionModelInfo);
		menuFile->addAction(actionSnapShot);
		menuFile->addSeparator();
		menuFile->addAction(actionOpenSession);
		menuFile->addAction(actionSaveSession);
		menuFile->addSeparator();
		menuFile->addAction(actionQuit);

		menuBar->addAction(menuEdit->menuAction());
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
		menuPost->addAction(actionSummary);
		menuPost->addAction(actionStats);
		menuPost->addAction(actionGraph);
		menuPost->addAction(actionIntegrate);

		menuBar->addAction(menuRecord->menuAction());
		menuRecord->addAction(actionRecordNew);
		menuRecord->addSeparator();
		menuRecord->addAction(actionRecordStart);
		menuRecord->addAction(actionRecordPause);
		menuRecord->addAction(actionRecordStop);

		menuBar->addAction(menuView->menuAction());
		menuView->addAction(actionViewSettings  );
		menuView->addAction(actionViewCapture   );
		menuView->addAction(actionViewProjection);
		menuView->addAction(actionViewMesh      );
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

		menuBar->addAction(menuHelp->menuAction());
		menuHelp->addAction(actionHelp);
		menuHelp->addAction(actionAbout);

		// Create the toolbar
		QToolBar* mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);

		mainToolBar->addAction(actionOpen);
		mainToolBar->addAction(actionSave);
		mainToolBar->addAction(actionUpdate);
	}
};

namespace Ui {
    class CMainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE
