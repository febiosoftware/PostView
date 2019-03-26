#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CSphereFitToolUI;

//-----------------------------------------------------------------------------
class CSphereFitTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CSphereFitTool(CMainWindow* wnd);

	// get the property list
	QWidget* createUi();

private slots:
	void OnFit();

private:
	CSphereFitToolUI*	ui;
	friend class Props;
};
