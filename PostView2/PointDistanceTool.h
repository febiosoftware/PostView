#pragma once
#include "Tool.h"
#include <PostViewLib/math3d.h>

class CDocument;
class CPointDistanceDecoration;

class CPointDistanceTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CPointDistanceTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CPointDistanceTool*	tool;
	};

public:
	CPointDistanceTool();

	CPropertyList* getPropertyList();

	void activate(CDocument* pdoc);

	void deactivate();

	void updateLength();

private:
	int			m_node1, m_node2;	// mesh nodes
	vec3f		m_d;				// displacement vector
	CDocument*	m_doc;
	CPointDistanceDecoration*	m_deco;

	friend class Props;
};
