#pragma once
#include "Tool.h"
#include <PostViewLib/math3d.h>

//-----------------------------------------------------------------------------
class CDocument;
class GPointDecoration;

//-----------------------------------------------------------------------------
// This tool measures the angle between three consecutively selected nodes
class CAddPointTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CAddPointTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CAddPointTool*	m_ptool;
	};

public:
	// constructor
	CAddPointTool();

	// get the property list
	CPropertyList*	getPropertyList();

	// activate the tool
	void activate(CDocument* pdoc);

	// deactive the tool
	void deactivate();

private:
	void UpdateNode();

private:
	vec3f	m_pos;

	CDocument*				m_doc;
	GPointDecoration*	m_deco;

	friend class Props;
};