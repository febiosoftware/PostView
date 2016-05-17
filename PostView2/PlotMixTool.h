#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;
class CPlotMixToolUI;
class CKinematToolUI;

class CPlotMixTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CPlotMixTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnBrowse();
	void OnRemove();
	void OnMoveUp();
	void OnMoveDown();
	void OnApply();

private:
	CDocument*			m_doc;
	CPlotMixToolUI*		ui;
	friend class Props;
};

class CKinematTool : public CAbstractTool
{
	Q_OBJECT

public:
	// constructor
	CKinematTool();

	// get the property list
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private slots:
	void OnBrowse1();
	void OnBrowse2();
	void OnApply();

private:
	CDocument*			m_doc;
	CKinematToolUI*		ui;
	friend class Props;
};
