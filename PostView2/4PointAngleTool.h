#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;
class C4PointAngleDecoration;

//-----------------------------------------------------------------------------
// This tool measures the angle between four consecutively selected nodes
class C4PointAngleTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(C4PointAngleTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		C4PointAngleTool*	m_ptool;
	};

public:
	// constructor
	C4PointAngleTool();

	// get the property list
	CPropertyList*	getPropertyList();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private:
	void UpdateAngle();

private:
	int		m_node[4];
	double	m_angle;

	CDocument*				m_doc;
	C4PointAngleDecoration*	m_deco;

	friend class Props;
};
