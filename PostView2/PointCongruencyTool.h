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
	CPointCongruencyTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnApply();

private:
	CDocument*				m_doc;
	CPointCongruencyToolUI*	ui;
	friend class Props;
};
