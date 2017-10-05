#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

//-----------------------------------------------------------------------------
// This tool measures the angle between three consecutively selected nodes
class CShellThicknessTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CShellThicknessTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CShellThicknessTool*	m_ptool;
	};

public:
	// constructor
	CShellThicknessTool(CDocument* doc);

	// get the property list
	CPropertyList*	getPropertyList();

	// Apply button
	void OnApply();

private:
	double	m_h;		// shell thickness

	friend class Props;
};
