#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

class CSphereFitToolUI;

//-----------------------------------------------------------------------------
class CSphereFitTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CSphereFitTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnFit();

private:
	CDocument*				m_doc;
	CSphereFitToolUI*	ui;
	friend class Props;
};
