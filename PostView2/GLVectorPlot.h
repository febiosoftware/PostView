// GLVectorPlot.h: interface for the CGLVectorPlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLVECTORPLOT_H__79896D5C_D71B_46BC_8BD8_729668134D62__INCLUDED_)
#define AFX_GLVECTORPLOT_H__79896D5C_D71B_46BC_8BD8_729668134D62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLPlot.h"

class CGLVectorPlot : public CGLPlot 
{
public:
	CGLVectorPlot(CGLModel* po);
	virtual ~CGLVectorPlot();

	void Render(CGLContext& rc);

	void SetScaleFactor(float g) { m_scale = g; }
	double GetScaleFactor() { return m_scale; }

	void SetDensity(float d) { m_dens = d; }
	double GetDensity() { return m_dens; }

	int GetVectorType() { return m_nvec; }
	void SetVectorType(int ntype);

	int GetGlyphType() { return m_nglyph; }
	void SetGlyphType(int ntype) { m_nglyph = ntype; }

	int GetColorType() { return m_ncol; }
	void SetColorType(int ntype) { m_ncol = ntype; }

	GLCOLOR GetGlyphColor() { return m_gcl; }
	void SetGlyphColor(GLCOLOR c) { m_gcl = c; }

	bool NormalizeVectors() { return m_bnorm; }
	void NormalizeVectors(bool b) { m_bnorm = b; }

	bool GetAutoScale() { return m_bautoscale; }
	void SetAutoScale(bool b) { m_bautoscale = b; }

	bool ShowHidden() const { return m_bshowHidden; }
	void ShowHidden(bool b) { m_bshowHidden = b; }

	CColorTexture* GetColorMap() { return &m_Col; }

	void Update(int ntime, float dt, bool breset);

	CPropertyList* propertyList();

protected:
	float	m_scale;
	float	m_dens;
	int		m_seed;

	int		m_nvec;		// vector field
	int		m_nglyph;	// glyph type
	int		m_ncol;		// color type

	bool	m_bnorm;		// normalize vectors or not
	bool	m_bautoscale;	// auto scale the vectors
	bool	m_bshowHidden;	// show vectors on hidden materials

	GLCOLOR			m_gcl;	// glyph color (for GLYPH_COL_SOLID)
	CColorTexture	m_Col;	// glyph color (for not GLYPH_COL_SOLID)

	vector<vec2f>	m_rng;	// nodal ranges
	DataMap<vec3f>	m_map;	// nodal values map
	
	int				m_ntime;	// current time at which this plot is evaluated
	vector<vec3f>	m_val;	// current nodal values
	vec2f			m_crng;	// current range
};

#endif // !defined(AFX_GLVECTORPLOT_H__79896D5C_D71B_46BC_8BD8_729668134D62__INCLUDED_)
