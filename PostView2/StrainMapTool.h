#pragma once
#include "Tool.h"

class CStrainMapToolUI;

// Tool that estimates strain (at surface) by calculating the overlap between surfaces. 
class CStrainMapTool : public CAbstractTool
{
	Q_OBJECT

public:
	CStrainMapTool(CMainWindow* wnd);

	// get the property list
	QWidget* createUi();

	private slots:
	void OnAssignFront1();
	void OnAssignBack1();
	void OnAssignFront2();
	void OnAssignBack2();
	void OnApply();

private:
	CStrainMapToolUI*		ui;
	friend class Props;
};
