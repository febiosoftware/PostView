#pragma once
#include "Tool.h"
#include <PostViewLib/math3d.h>

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
	CTransformTool();

	// get the property list
	CPropertyList*	getPropertyList();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

	// Apply button
	void OnApply();

private:
	vec3f	m_dr;

	CDocument*				m_doc;

	friend class Props;
};
