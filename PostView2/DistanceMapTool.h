#pragma once
#include "Tool.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class CDocument;

class CDistanceMapToolUI;

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

private:
	void get_selection(vector<int>& sel);

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CDocument*				m_doc;
	CDistanceMapToolUI*		ui;
	friend class Props;
};
