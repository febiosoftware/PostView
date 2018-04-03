#pragma once
#include "GLPlot.h"

class CGLStreamLinePlot : public CGLPlot
{
	struct StreamPoint
	{
		vec3f		r;
		float		v;
		GLCOLOR		c;
	};

	class StreamLine
	{
	public:
		StreamLine(){}
		StreamLine(const StreamLine& sl)
		{
			m_pt = sl.m_pt;
		}
		void operator = (const StreamLine& sl)
		{
			m_pt = sl.m_pt;
		}

		void Add(const vec3f& r, float w) { StreamPoint p = {r, w, GLCOLOR(0,0,0)}; m_pt.push_back(p); }

		int Points() const { return (int) m_pt.size(); }
		StreamPoint& operator [] (int i) { return m_pt[i]; }

		vector<StreamPoint>	m_pt;
	};

public:
	CGLStreamLinePlot(CGLModel* fem);

	void Render(CGLContext& rc);

	void Update(int ntime, float dt, bool breset);

	CPropertyList* propertyList();

	void UpdateStreamLines();

	void ColorStreamLines();

public:
	int GetVectorType() { return m_nvec; }
	void SetVectorType(int ntype);

	CColorTexture* GetColorMap() { return &m_Col; }

	float StepSize() const { return m_inc; }
	void SetStepSize(float v) { m_inc = v; }

protected:

	vec3f Velocity(const vec3f& r, bool& ok);

private:
	int	m_nvec;	// vector field

	float	m_inc;

	CColorTexture	m_Col;	// color map

	vector<vec2f>	m_rng;	// nodal ranges
	DataMap<vec3f>	m_map;	// nodal values map

	int				m_ntime;	// current time at which this plot is evaluated
	vector<vec3f>	m_val;	// current nodal values
	vec2f			m_crng;	// current range

	vector<StreamLine>	m_streamLines;

	FEFindElement	m_find;
};
