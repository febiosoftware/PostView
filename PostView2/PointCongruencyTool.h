#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

class CPointCongruencyToolUI;

//-----------------------------------------------------------------------------
class CPointCongruencyTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CPointCongruencyTool(CDocument* doc);

	// get the property list
	QWidget* createUi();

private slots:
	void OnApply();

private:
	CPointCongruencyToolUI*	ui;
	friend class Props;
};
