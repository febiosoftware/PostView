#pragma once
#include "Tool.h"

class CDocument;
class CPlaneToolUI;
class CPlaneDecoration;

class CPlaneTool : public CAbstractTool
{
	Q_OBJECT

public:
	CPlaneTool();

	// get the UI widget
	QWidget* createUi();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private:
	void UpdateNormal();

private slots:
	void on_change_node1();
	void on_change_node2();
	void on_change_node3();
	void onAlignView();

private:
	CDocument*			m_doc;
	CPlaneToolUI*		ui;
	CPlaneDecoration*	m_dec;
};
