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
	CAddImage3DTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnApply();
	void OnBrowse();

private:
	CDocument*				m_doc;
	CAddImage3DToolUI*		ui;
	friend class Props;
};
