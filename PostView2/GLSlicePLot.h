// GLSlicePLot.h: interface for the CGLSlicePLot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLSLICEPLOT_H__1B8F12EA_87A7_415A_9774_1033393B4434__INCLUDED_)
#define AFX_GLSLICEPLOT_H__1B8F12EA_87A7_415A_9774_1033393B4434__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLPlot.h"
#include "GLWidget.h"
#include "PostViewLib/DataMap.h"

class CGLSlicePlot : public CGLPlot  
{
public:
	CGLSlicePlot(CGLModel* po);
	virtual ~CGLSlicePlot();

	void SetBoundingBox(BOUNDINGBOX box) { m_box = box; }

	int GetSlices();
	void SetSlices(int nslices);

	vec3f GetPlaneNormal() { return m_norm; }
	void SetPlaneNormal(vec3f& n) { m_norm = n; }

	void Render(CGLContext& rc);

	int GetEvalField() { return m_nfield; }
	void SetEvalField(int n);

	CColorTexture* GetColorMap() { return &m_Col; }

	void Update(int ntime, float dt, bool breset);

	bool ShowLegend() { return m_pbar->visible(); }
	void ShowLegend(bool b) { if (b) m_pbar->show(); else m_pbar->hide(); }

	void SetRangeType(int ntype) { m_nrange = ntype; }
	int GetRangeType() const { return m_nrange; }

	float GetUserRangeMax() const { return m_fmax; }
	void SetUserRangeMax(float f) { m_fmax = f; }
	float GetUserRangeMin() const { return m_fmin; }
	void SetUserRangeMin(float f) { m_fmin = f; }

	CPropertyList* propertyList();

protected:
	void RenderSlice(float ref);

protected:
	int			m_nslices;	// nr. of iso surface slices
	BOUNDINGBOX	m_box;		// box to use for slices	
	vec3f		m_norm;

	int			m_nrange;		//!< range option (0=dynamic, 1=user)
	float		m_fmin, m_fmax;	//!< user-defined range 

	int				m_nfield;	// data field
	CColorTexture	m_Col;		// colormap

	vector<vec2f>	m_rng;	// value range
	DataMap<float>	m_map;	// nodal values map
	vector<float>	m_val;	// current nodal values
	vec2f			m_crng;	// current range

	GLLegendBar*	m_pbar;
};

#endif // !defined(AFX_GLSLICEPLOT_H__1B8F12EA_87A7_415A_9774_1033393B4434__INCLUDED_)
