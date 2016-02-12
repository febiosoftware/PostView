// GLIsoSurfacePlot.h: interface for the CGLIsoSurfacePlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLISOSURFACEPLOT_H__465A729C_004B_442C_BF28_6E11675D3B81__INCLUDED_)
#define AFX_GLISOSURFACEPLOT_H__465A729C_004B_442C_BF28_6E11675D3B81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLPlot.h"
#include "GLWidget.h"
#include "PostViewLib/DataMap.h"

class CGLIsoSurfacePlot : public CGLPlot  
{
public:
	CGLIsoSurfacePlot(CGLModel* po);
	virtual ~CGLIsoSurfacePlot();

	int GetSlices() { return m_nslices; }
	void SetSlices(int nslices) { m_nslices = nslices; m_Col.SetDivisions(nslices, false); }

	void Render(CGLContext& rc);

	void Update(int ntime, float dt, bool breset);

	bool RenderSmooth() { return m_bsmooth; }
	void RenderSmooth(bool b) { m_bsmooth = b; }

	int GetEvalField() { return m_nfield; }
	void SetEvalField(int n) { m_nfield = n; }

	CColorMap* GetColorMap() { return &m_Col; }

	bool ShowLegend() { return m_pbar->visible(); }
	void ShowLegend(bool b) { if (b) m_pbar->show(); else m_pbar->hide(); }

	bool CutHidden() { return m_bcut_hidden; }
	void CutHidden(bool b) { m_bcut_hidden = b; }

protected:
	void RenderSlice(float ref);

protected:
	int		m_nslices;		// nr. of iso surface slices
	bool	m_bsmooth;		// render smooth or not
	bool	m_bcut_hidden;	//!< cut hidden materials or not

	int			m_nfield;	// data field
	CColorMap	m_Col;		// colormap

	vector<vec2f>	m_rng;	// value range
	DataMap<float>	m_map;	// nodal values map
	VectorMap		m_GMap;	// nodal gradient values map

	vec2f			m_crng;
	vector<float>	m_val;	// current nodal values
	vector<vec3f>	m_grd;	// current gradient values

	GLLegendBar*	m_pbar;
};

#endif // !defined(AFX_GLISOSURFACEPLOT_H__465A729C_004B_442C_BF28_6E11675D3B81__INCLUDED_)
