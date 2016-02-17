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

QT_BEGIN_NAMESPACE

class CMyComboBox : public QComboBox
{
public:
	CMyComboBox(QWidget* parent = 0) : QComboBox(parent){}

//	void hidePopup(){}
};

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

	QAction*	actionColorMap;

	CFileViewer*	fileViewer;
	CModelViewer*	modelViewer;
	CMaterialPanel*	matPanel;
	CDataPanel*		dataPanel;
	CStatePanel*	statePanel;
	CToolsPanel*	toolsPanel;
	CGLView*		glview;

	CDataFieldSelector*	selectData;

public:
	// build the UI
	void setupUi(CMainWindow* MainWindow)
	{
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
        QDockWidget* dock1 = new QDockWidget("Files", MainWindow);
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        fileViewer = new CFileViewer(MainWindow, dock1);
        fileViewer->setObjectName(QStringLiteral("fileViewer"));
        dock1->setWidget(fileViewer);
		MainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock1);
		menuView->addAction(dock1->toggleViewAction());

		QDockWidget* dock2 = new QDockWidget("Model", MainWindow);
        dock1->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		modelViewer = new CModelViewer(MainWindow, dock2);
		dock2->setWidget(modelViewer);
		menuView->addAction(dock2->toggleViewAction());
		MainWindow->tabifyDockWidget(dock1, dock2);

		QDockWidget* dock3 = new QDockWidget("Materials", MainWindow);
        dock3->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		matPanel = new CMaterialPanel(MainWindow, dock3);
		dock3->setWidget(matPanel);
		menuView->addAction(dock3->toggleViewAction());
		MainWindow->tabifyDockWidget(dock2, dock3);

		QDockWidget* dock4 = new QDockWidget("Data", MainWindow);
        dock4->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		dataPanel = new CDataPanel(MainWindow, dock4);
		dock4->setWidget(dataPanel);
		menuView->addAction(dock4->toggleViewAction());
		MainWindow->tabifyDockWidget(dock3, dock4);

		QDockWidget* dock5 = new QDockWidget("State", MainWindow);
        dock5->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
		statePanel = new CStatePanel(MainWindow, dock5);
		dock5->setWidget(statePanel);
		menuView->addAction(dock5->toggleViewAction());
		MainWindow->tabifyDockWidget(dock4, dock5);

		QDockWidget* dock6 = new QDockWidget("Tools", MainWindow);
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
		QAction* actionOpen        = addAction("Open ..."         , "actionOpen"  , ":/icons/open.png");
		QAction* actionSave        = addAction("save. .."         , "actionSave"  , ":/icons/save.png");
		QAction* actionUpdate      = addAction("Update"           , "actionUpdate", ":/icons/refresh.png");
		QAction* actionExport      = addAction("Export ..."       , "actionExport");
		QAction* actionModelInfo   = addAction("Model Info ..."   , "actionModelInfo"  );
		QAction* actionSnapShot    = addAction("Snapshot ..."     , "actionSnapShot"   );
		QAction* actionOpenSession = addAction("Open session ..." , "actionOpenSession");
		QAction* actionSaveSession = addAction("Save session ..." , "actionSaveSession");
		QAction* actionQuit        = addAction("Exit"             , "actionQuit"       );

		// --- Edit menu ---
		QAction* actionHideSelected    = addAction("Hide selected"   , "actionHideSelected"   );
		QAction* actionHideUnselected  = addAction("Hide unselected" , "actionHideUnselected" );
		QAction* actionInvertSelection = addAction("Invert selection", "actionInvertSelection");
		QAction* actionUnhideAll       = addAction("Unhide all      ", "actionUnhideAll"      );
		QAction* actionSelectAll       = addAction("Select all      ", "actionSelectAll"      );
		QAction* actionSelectRange     = addAction("Select range ...", "actionSelectRange"    );
		QAction* actionClearSelection  = addAction("Clear selection" , "actionClearSelection" );
		QAction* actionFind            = addAction("Find ..."        , "actionFind"           );
		QAction* actionDelete          = addAction("Delete ..."      , "actionDelete"         );
		QAction* actionProperties      = addAction("Properties ..."  , "actionProperties"     );

		// --- Post menu ---
		actionColorMap        = addAction("Toggle colormap" , "actionColorMap"       , ":/icons/colormap.png");
		QAction* actionPlaneCut        = addAction("Plane cut"       , "actionPlaneCut"       );
		QAction* actionVectorPlot      = addAction("Vector plot"     , "actionVectorPlot"     );
		QAction* actionIsosurfacePlot  = addAction("Isosurface plot" , "actionIsosurfacePlot" );
		QAction* actionSlicePlot       = addAction("Slice plot"      , "actionSlicePlot"      );
		QAction* actionDisplacementMap = addAction("Displacement map", "actionDisplacementMap");
		QAction* actionSummary         = addAction("Summary ..."     , "actionSummary"        );
		QAction* actionStats           = addAction("Statistics  ..." , "actionStats"          );
		QAction* actionGraph           = addAction("Graph ..."       , "actionGraph"          );
		QAction* actionIntegrate       = addAction("Integrate ..."   , "actionIntegrate"      );

		actionColorMap->setCheckable(true);
		actionColorMap->setDisabled(true);

		// --- Record menu ---
		QAction* actionRecordNew   = addAction("New ...", "actionRecordNew"  );
		QAction* actionRecordStart = addAction("Start"  , "actionRecordStart");
		QAction* actionRecordPause = addAction("Pause"  , "actionRecordPause");
		QAction* actionRecordStop  = addAction("Stop"   , "actionRecordStop" );

		// --- View Menu ---
		QAction* actionViewSettings   = addAction("Settings ..."          , "actionViewSettings"  );
		QAction* actionViewCapture    = addAction("Toggle capture Frame"  , "actionViewCapture"   );
		QAction* actionViewProjection = addAction("Toggle projection mode", "actionViewProjection");
		QAction* actionViewMesh       = addAction("Toggle mesh lines",      "actionViewMesh"      );
		QAction* actionViewOutline    = addAction("Toggle outline",         "actionViewOutline"   );
		QAction* actionViewSmooth     = addAction("Toggle color smoothing", "actionViewSmooth"    );
		QAction* actionViewFront      = addAction("Front",                  "actionViewFront"     );
		QAction* actionViewBack       = addAction("Back" ,                  "actionViewBack"      );
		QAction* actionViewLeft       = addAction("Left" ,                  "actionViewLeft"      );
		QAction* actionViewRight      = addAction("Right",                  "actionViewRight"     );
		QAction* actionViewTop        = addAction("Top" ,                   "actionViewTop"       );
		QAction* actionViewBottom     = addAction("Bottom" ,                "actionViewBottom"    );
		QAction* actionViewTrack      = addAction("Track selection",        "actionViewTrack"     );
		QAction* actionViewVPSave     = addAction("Save viewpoint",         "actionViewVPSave"    );
		QAction* actionViewVPPrev     = addAction("Prev viewpoint",         "actionViewVPPrev"    );
		QAction* actionViewVPNext     = addAction("Next viewpoint",         "actionViewVPNext"    );

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
		mainToolBar->addSeparator();

		// create the data field selector
		selectData = new CDataFieldSelector;
		selectData->setObjectName("selectData");

		mainToolBar->addWidget(selectData);
		mainToolBar->addAction(actionColorMap);
	}
};

namespace Ui {
    class CMainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE
