#pragma once
#include "Tool.h"

class CAreaCoverageToolUI;

class CAreaCoverageTool : public CAbstractTool
{
	Q_OBJECT

public:
	CAreaCoverageTool(CDocument* doc);

	QWidget* createUi();

private slots:
	void OnAssign1();
	void OnAssign2();
	void OnApply();

private:
	CAreaCoverageToolUI*	ui;
};
