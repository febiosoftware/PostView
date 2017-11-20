#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
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
	C3PointAngleTool(CDocument* doc);

	// get the property list
	CPropertyList*	getPropertyList();

	// activate the tool
	void activate();

	// deactive the tool
	void deactivate();

	void update(bool reset);

private:
	void UpdateAngle();

private:
	int		m_node[3];
	double	m_angle;

	C3PointAngleDecoration*	m_deco;

	friend class Props;
};
