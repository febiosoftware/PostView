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
	CDistanceMapTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CDocument*				m_doc;
	CDistanceMapToolUI*		ui;
	friend class Props;
};

//-----------------------------------------------------------------------------
class CCurvatureMapTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CCurvatureMapTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CDocument*				m_doc;
	CCurvatureMapToolUI*	ui;
	friend class Props;
};
