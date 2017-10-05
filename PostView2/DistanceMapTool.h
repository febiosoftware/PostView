#pragma once
#include "Tool.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class CDocument;

class CDistanceMapToolUI;
class CCurvatureMapToolUI;

//-----------------------------------------------------------------------------
class CDistanceMapTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CDistanceMapTool(CDocument* doc);

	// get the property list
	QWidget* createUi();

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CDistanceMapToolUI*		ui;
	friend class Props;
};

//-----------------------------------------------------------------------------
class CCurvatureMapTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CCurvatureMapTool(CDocument* doc);

	// get the property list
	QWidget* createUi();

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CCurvatureMapToolUI*	ui;
	friend class Props;
};
