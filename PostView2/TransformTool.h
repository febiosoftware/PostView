#pragma once
#include "Tool.h"
#include <MathLib/math3d.h>

//-----------------------------------------------------------------------------
class CDocument;

//-----------------------------------------------------------------------------
class CTransformTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CTransformTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CTransformTool*	m_ptool;
	};

public:
	// constructor
	CTransformTool(CMainWindow* wnd);

	// get the property list
	CPropertyList*	getPropertyList();

	// Apply button
	void OnApply();

private:
	vec3f	m_dr;

	friend class Props;
};
