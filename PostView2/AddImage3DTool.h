#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

class CAddImage3DToolUI;

//-----------------------------------------------------------------------------
class CAddImage3DTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CAddImage3DTool(CMainWindow* wnd);

	// get the property list
	QWidget* createUi();

private slots:
	void OnApply();
	void OnBrowse();

private:
	CAddImage3DToolUI*		ui;
	friend class Props;
};
