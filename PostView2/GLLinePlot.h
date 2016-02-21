#pragma once

#include "GLPlot.h"

//-----------------------------------------------------------------------------
// Line rendering of imported line data
class CGLLinePlot : public CGLPlot
{
public:
	CGLLinePlot(CGLModel* po);
	virtual ~CGLLinePlot();

	void Render(CGLContext& rc);

	float GetLineWidth() { return m_line; }
	void SetLineWidth(float f) { m_line = f; }

	GLCOLOR GetLineColor() { return m_col; }
	void SetLineColor(GLCOLOR c) { m_col = c; }

	int GetRenderMode() { return m_nmode; }
	void SetRenderMode(int m) { m_nmode = m; }

	CPropertyList* propertyList();

protected:
	void RenderLines(FEState& s);
	void Render3DLines(FEState& s);

private:
	float		m_line;		//!< line thickness
	GLCOLOR		m_col;		//!< rendering color
	int			m_nmode;	//!< rendering mode
};

//-----------------------------------------------------------------------------
// point cloud rendering of imported point data
class CGLPointPlot : public CGLPlot
{
	enum {MAX_SETTINGS = 4};

	struct SETTINGS
	{
		float		size;		//!< point size
		GLCOLOR		col;		//!< rendering color
		int			nmode;		//!< rendering mode
		int			nvisible;	//!< visible flag
	};

public:
	CGLPointPlot(CGLModel* po);
	virtual ~CGLPointPlot();

	CPropertyList* propertyList();

	void Render(CGLContext& rc);

	float GetPointSize(int n = 0) { return m_set[n].size; }
	void SetPointSize(float f, int n = 0) { m_set[n].size = f; }

	GLCOLOR GetPointColor(int n = 0) { return m_set[n].col; }
	void SetPointColor(GLCOLOR c, int n = 0) { m_set[n].col = c; }

	int GetRenderMode(int n = 0) { return m_set[n].nmode; }
	void SetRenderMode(int m, int n = 0) { m_set[n].nmode = m; }

	int GetVisible(int n = 0) { return m_set[n].nvisible; }
	void SetVisible(int nvis, int n = 0) { m_set[n].nvisible = nvis; }

private:
	SETTINGS	m_set[MAX_SETTINGS];
};
