#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

class CImportLinesToolUI;
class CImportPointsToolUI;

//-----------------------------------------------------------------------------
class CImportLinesTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CImportLinesTool();

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
	bool ReadOldFormat(const char* szfile);
	bool ReadAng2Format(const char* szfile);

private:
	CDocument*				m_doc;
	CImportLinesToolUI*		ui;
	friend class Props;
};

//-----------------------------------------------------------------------------
class CImportPointsTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CImportPointsTool();

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
	CImportPointsToolUI*	ui;
	friend class Props;
};
