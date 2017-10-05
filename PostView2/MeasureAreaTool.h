#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

//-----------------------------------------------------------------------------
// This tool measures the angle between three consecutively selected nodes
class CMeasureAreaTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CMeasureAreaTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CMeasureAreaTool*	m_ptool;
	};

public:
	// constructor
	CMeasureAreaTool(CDocument* doc);

	// get the property list
	CPropertyList*	getPropertyList();

	// Apply button
	void OnApply();

private:
	int		m_nsel;		// selected faces
	double	m_area;		// area of selection

	friend class Props;
};
