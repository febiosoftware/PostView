#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;
class C3PointAngleDecoration;

//-----------------------------------------------------------------------------
// This tool measures the angle between three consecutively selected nodes
class C3PointAngleTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(C3PointAngleTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		C3PointAngleTool*	m_ptool;
	};

public:
	// constructor
	C3PointAngleTool();

	// get the property list
	CPropertyList*	getPropertyList();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private:
	void UpdateAngle();

private:
	int		m_node[3];
	double	m_angle;

	CDocument*				m_doc;
	C3PointAngleDecoration*	m_deco;

	friend class Props;
};
