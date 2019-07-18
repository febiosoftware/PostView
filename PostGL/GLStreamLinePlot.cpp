#include "stdafx.h"
#include "GLStreamLinePlot.h"
#include <PostView2/PropertyList.h>
#include <GLWLib/GLWidgetManager.h>
using namespace Post;

class CStreamLinePlotProps : public CPropertyList
{
public:
	CStreamLinePlotProps(CGLStreamLinePlot* v) : m_plt(v)
	{
		QStringList cols;

		for (int i = 0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		addProperty("Data field"    , CProperty::DataVec3);
		addProperty("Color map"     , CProperty::Enum)->setEnumValues(cols);
		addProperty("Allow clipping", CProperty::Bool);
		addProperty("Step size"     , CProperty::Float);
		addProperty("Density"       , CProperty::Float)->setFloatRange(0.0, 1.0).setFloatStep(0.01);
		addProperty("Velocity threshold", CProperty::Float);
		addProperty("Range Type", CProperty::Enum)->setEnumValues(QStringList() << "Dynamic" << "Static" << "User");
		addProperty("Range divisions", CProperty::Int)->setIntRange(1, 100);
		addProperty("User Range Min", CProperty::Float);
		addProperty("User Range Max", CProperty::Float);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_plt->GetVectorType(); break;
		case 1: return m_plt->GetColorMap()->GetColorMap();
		case 2: return m_plt->AllowClipping(); break;
		case 3: return m_plt->StepSize(); break;
		case 4: return m_plt->Density(); break;
		case 5: return m_plt->Threshold(); break;
		case 6: return m_plt->GetRangeType(); break;
		case 7: return m_plt->GetColorMap()->GetDivisions(); break;
		case 8: return m_plt->GetUserRangeMin(); break;
		case 9: return m_plt->GetUserRangeMax(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_plt->SetVectorType(v.toInt()); break;
		case 1: m_plt->GetColorMap()->SetColorMap(v.toInt()); m_plt->ColorStreamLines(); break;
		case 2: m_plt->AllowClipping(v.toBool()); break;
		case 3: m_plt->SetStepSize(v.toFloat()); m_plt->UpdateStreamLines(); break; 
		case 4: m_plt->SetDensity(v.toFloat()); m_plt->UpdateStreamLines(); break;
		case 5: m_plt->SetThreshold(v.toFloat()); break;
		case 6: m_plt->SetRangeType(v.toInt()); break;
		case 7: m_plt->GetColorMap()->SetDivisions(v.toInt()); break;
		case 8: m_plt->SetUserRangeMin(v.toFloat()); break;
		case 9: m_plt->SetUserRangeMax(v.toFloat()); break;
		}

		m_plt->Update();
	}

private:
	CGLStreamLinePlot*	m_plt;
};

//=================================================================================================

CGLStreamLinePlot::CGLStreamLinePlot(CGLModel* fem) : CGLPlot(fem), m_find(*fem->GetActiveMesh())
{
	static int n = 1;
	char szname[128] = { 0 };
	sprintf(szname, "StreamLines.%02d", n++);
	SetName(szname);

	m_density = 1.f;
	m_vtol = 1e-5f;

	m_nvec = -1;
	m_inc = 0.01f;

	m_rangeType = RNG_DYNAMIC;
	m_userMin = 0.;
	m_userMax = 1.;
	m_rngMin = 0.;
	m_rngMax = 1.;

	m_lastTime = 0;
	m_lastdt = 1.f;

	m_Col.SetDivisions(10);

	m_pbar = new GLLegendBar(&m_Col, 0, 0, 600, 100, GLLegendBar::HORIZONTAL);
	m_pbar->align(GLW_ALIGN_BOTTOM | GLW_ALIGN_HCENTER);
	m_pbar->SetType(GLLegendBar::GRADIENT);
	m_pbar->copy_label(szname);
	m_pbar->ShowTitle(true);
	CGLWidgetManager::GetInstance()->AddWidget(m_pbar);
}

CGLStreamLinePlot::~CGLStreamLinePlot()
{
	CGLWidgetManager::GetInstance()->RemoveWidget(m_pbar);
	delete m_pbar;
}

CPropertyList* CGLStreamLinePlot::propertyList()
{
	return new CStreamLinePlotProps(this);
}

void CGLStreamLinePlot::Update()
{
	Update(m_lastTime, m_lastdt, true);
}

void CGLStreamLinePlot::SetVectorType(int ntype)
{
	m_nvec = ntype;

	FEModel* fem = GetModel()->GetFEModel();
	std::string dataName = fem->getDataString(m_nvec, DATA_VECTOR);
	m_pbar->copy_label(dataName.c_str());

	Update();
}

void CGLStreamLinePlot::Render(CGLContext& rc)
{
	int NSL = (int)m_streamLines.size();
	if (NSL == 0) return;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);

	for (int i=0; i<NSL; ++i)
	{
		StreamLine& sl = m_streamLines[i];

		int NP = sl.Points();
		if (NP > 1)
		{
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j<NP; ++j)
			{
				StreamPoint& pt = sl[j];
				GLColor& c = pt.c;
				vec3f& r = pt.r;

				glColor3ub(c.r, c.g, c.b);
				glVertex3f(r.x, r.y, r.z);
			}
			glEnd();
		}
	}

	glPopAttrib();
}

static float frand()
{
	return rand() / (float)RAND_MAX;
}

void CGLStreamLinePlot::Update(int ntime, float dt, bool breset)
{
	m_lastTime = ntime;
	m_lastdt = dt;

	CGLModel* mdl = GetModel();
	FEMeshBase* pm = mdl->GetActiveMesh();
	FEModel* pfem = mdl->GetFEModel();

	if (breset) { m_map.Clear(); m_rng.clear(); m_val.clear(); m_prob.clear(); }

	// see if we need to revaluate the FEFindElement object
	// We evaluate it when the plot needs to be reset, or when the model has a displacement map
	bool bdisp = mdl->HasDisplacementMap();
	if (breset || bdisp)
	{
		// choose reference frame or current frame, depending on whether we have a displacement map
		m_find.Init(bdisp ? 1 : 0);
	}

	if (m_map.States() == 0)
	{
		int NS = pfem->GetStates();
		int NN = pm->Nodes();

		m_map.Create(NS, NN, vec3f(0, 0, 0), -1);
		m_rng.resize(NS);
		m_val.resize(NN);
	}

	if (m_prob.empty())
	{
		FEMeshBase& mesh = *pfem->GetFEMesh(0);
		int NF = mesh.Faces();
		m_prob.resize(NF);
		for (int i=0; i<NF; ++i) m_prob[i] = frand();
	}

	// check the tag
	int ntag = m_map.GetTag(ntime);

	// see if we need to update
	if (ntag != m_nvec)
	{
		m_map.SetTag(ntime, m_nvec);

		// get the state we are interested in
		vector<vec3f>& val = m_map.State(ntime);

		vec2f& rng = m_rng[ntime];
		rng.x = rng.y = 0;

		float L;

		for (int i = 0; i<pm->Nodes(); ++i)
		{
			val[i] = pfem->EvaluateNodeVector(i, ntime, m_nvec);
			L = val[i].Length();
			if (L > rng.y) rng.y = L;
		}

		if (rng.y == rng.x) ++rng.y;
	}

	// copy nodal values
	m_val = m_map.State(ntime);
	m_crng = m_rng[ntime];

	// update static range
	if (breset) { m_rngMin = m_crng.x; m_rngMax = m_crng.y; }
	else
	{
		if (m_crng.x < m_rngMin) m_rngMin = m_crng.x;
		if (m_crng.y > m_rngMax) m_rngMax = m_crng.y;
	}

	// set range for color map
	switch (m_rangeType)
	{
	case RNG_DYNAMIC:
		m_crng = m_rng[ntime];
		break;
	case RNG_STATIC:
		m_crng = vec2f((float)m_rngMin, (float)m_rngMax);
		break;
	case RNG_USER:
		m_crng = vec2f((float)m_userMin, (float)m_userMax);
		break;
	}

	m_pbar->SetRange(m_crng.x, m_crng.y);

	// update stream lins
	UpdateStreamLines();
}

vec3f CGLStreamLinePlot::Velocity(const vec3f& r, bool& ok)
{
	vec3f v(0.f, 0.f, 0.f);
	vec3f ve[FEGenericElement::MAX_NODES];
	FEMeshBase& mesh = *GetModel()->GetActiveMesh();
	int nelem;
	double q[3];
	if (m_find.FindElement(r, nelem, q))
	{
		ok = true;
		FEElement& el = mesh.Element(nelem);

		int ne = el.Nodes();
		for (int i=0; i<ne; ++i) ve[i] = m_val[el.m_node[i]];

		v = el.eval(ve, q[0], q[1], q[2]);
	}
	else ok = false;

	return v;
}

void CGLStreamLinePlot::UpdateStreamLines()
{
	// clear current stream lines
	m_streamLines.clear();

	// make sure we have a valid vector field
	if (m_nvec == -1) return;

	// get the model
	CGLModel* mdl = GetModel();
	if (mdl == 0) return;

	if (m_inc < 1e-6) return;

	int MAX_POINTS = 2*(int)(1.f / m_inc);
	if (MAX_POINTS > 10000) MAX_POINTS = 10000;

	// make sure vtol is positive
	float vtol = fabs(m_vtol);

	// get the mesh
	FEMeshBase& mesh = *mdl->GetActiveMesh();

	BOUNDINGBOX box = m_find.BoundingBox();
	float R = box.GetMaxExtent();
	float maxStep = m_inc*R;

	// tag all elements
	mesh.SetElementTags(0);

	// use the same seed
	srand(0);

	// loop over all the surface facts
	double q[3];
	int NF = mesh.Faces();
	for (int i=0; i<NF; ++i)
	{
		FEFace& f = mesh.Face(i);

		// evaluate the average velocity at this face
		int nf = f.Nodes();
		vec3f vf(0.f, 0.f, 0.f);
		for (int j=0; j<nf; ++j) vf += m_val[f.node[j]];
		vf /= nf;

		// see if this is a valid candidate for a seed
		vec3f fn = f.m_fn;
		if ((fn*vf < -vtol) && (m_prob[i] <= m_density))
		{
			// calculate the face center, this will be the seed
			// NOTE: We are using reference coordinates, therefore we assume that te mesh is not deforming!!
			vec3f cf(0.f, 0.f, 0.f);
			for (int j = 0; j<nf; ++j) cf += mesh.Node(f.node[j]).m_r0;
			cf /= nf;

			// project the seed into the adjacent solid element
			int nelem = f.m_elem[0];
			FEElement* el = &mesh.Element(nelem);
			el->m_ntag = 1;
			ProjectInsideReferenceElement(mesh, *el, cf, q);

			double V = vf.Length();

			// now, propagate the seed and form the stream line
			StreamLine l;
			l.Add(cf, V);

			vec3f vc = vf;

			vec3f v[FEGenericElement::MAX_NODES];
			bool ok;
			do
			{
				// make sure the velocity is not zero, otherwise we'll be stuck
				double V = vc.Length();
				if (V < 1e-5f) break;

				// "time" increment
				float dt =  maxStep / V;

				// propagate the seed
				// Euler's method
//				cf += vc*dt;

				// RK2 method
/*				vec3f p = cf + vc*dt;
				vec3f vp = Velocity(p, ok);
				if (ok == false) break;
				cf += (vc + vp)*(dt*0.5f);
*/
				// RK4
				vec3f dr(0.f, 0.f, 0.f);
				do
				{
					vec3f a = vc*dt;
					vec3f b = Velocity(cf + a*0.5f, ok)*dt; if (ok == false) break;
					vec3f c = Velocity(cf + b*0.5f, ok)*dt; if (ok == false) break;
					vec3f d = Velocity(cf + c     , ok)*dt; if (ok == false) break;

					dr = (a + b*2.f + c*2.f + d) / 6.0;
					float DR = dr.Length();
					if (DR > 2.0f*maxStep) dt *= 0.5f; else break;
				}
				while (1);
				if (ok == false) break;

				cf += dr;

				// add it to the stream line
				l.Add(cf, V);

				// if for some reason we're stuck, we'll set a max nr of points
				if (l.Points() > MAX_POINTS) break;

				// get velocity at new point
				vc = Velocity(cf, ok);
				if (ok == false) break;
			}
			while (1);

			if (l.Points() > 2)
			{
				m_streamLines.push_back(l);
			}
		}
	}

	// evaluate the color of stream lines
	ColorStreamLines();
}

void CGLStreamLinePlot::ColorStreamLines()
{
	// get the range
	float Vmin = m_crng.x;
	float Vmax = m_crng.y;
	if (Vmax == Vmin) Vmax++;

	int ncol = m_Col.GetColorMap();
	CColorMap& col = ColorMapManager::GetColorMap(ncol);

	int NSL = (int)m_streamLines.size();
	for (int i=0; i<NSL; ++i)
	{
		StreamLine& sl = m_streamLines[i];
		int NP = sl.Points();
		for (int j=0; j<NP; ++j)
		{
			StreamPoint& pt = sl[j];

			float V = pt.v;
			float w = (V - Vmin) / (Vmax - Vmin);
			pt.c = col.map(w);
		}
	}
}
