#pragma once
#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

//-----------------------------------------------------------------------------
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
	CShellThicknessTool(CMainWindow* wnd);

	// get the property list
	CPropertyList*	getPropertyList();

	// Apply button
	void OnApply();

private:
	double	m_h;		// shell thickness

	friend class Props;
};
