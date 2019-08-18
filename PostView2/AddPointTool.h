#pragma once
#include "Tool.h"
#include <PostLib/math3d.h>

//-----------------------------------------------------------------------------
class CDocument;
class GPointDecoration;

//-----------------------------------------------------------------------------
// This tool measures the angle between three consecutively selected nodes
class CAddPointTool : public CBasicTool
{
	class Props : public Post::CPropertyList
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
	CAddPointTool(CMainWindow* wnd);

	// get the property list
	Post::CPropertyList*	getPropertyList();

	// activate the tool
	void activate();

	// deactive the tool
	void deactivate();

private:
	void UpdateNode();

private:
	vec3f	m_pos;

	GPointDecoration*	m_deco;

	friend class Props;
};
