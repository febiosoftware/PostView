#include "stdafx.h"
#include "GLParticleFlowPlot.h"
#include "PropertyList.h"

class CGLParticleFlowPlotProps : public CPropertyList
{
public:
	CGLParticleFlowPlotProps(CGLParticleFlowPlot* v) : m_plt(v)
	{
		QStringList cols;

		for (int i = 0; i<ColorMapManager::ColorMaps(); ++i)
		{
			string name = ColorMapManager::GetColorMapName(i);
			cols << name.c_str();
		}

		addProperty("Data field", CProperty::DataVec3);
		addProperty("Color map", CProperty::Enum)->setEnumValues(cols);
		addProperty("Allow clipping", CProperty::Bool);
		addProperty("Seed step", CProperty::Int);
		addProperty("Velocity threshold", CProperty::Float);
		addProperty("Seeding density", CProperty::Float)->setFloatRange(0.0, 1.0).setFloatStep(0.01);
		addProperty("Step size", CProperty::Float);
		addProperty("Show path lines", CProperty::Bool);
	}

	QVariant GetPropertyValue(int i)
	{
		switch (i)
		{
		case 0: return m_plt->GetVectorType(); break;
		case 1: return m_plt->GetColorMap()->GetColorMap();
		case 2: return m_plt->AllowClipping(); break;
		case 3: return m_plt->SeedTime() + 1; break;
		case 4: return m_plt->Threshold(); break;
		case 5: return m_plt->Density(); break;
		case 6: return m_plt->StepSize(); break;
		case 7: return m_plt->ShowPath(); break;
		}
		return QVariant();
	}

	void SetPropertyValue(int i, const QVariant& v)
	{
		switch (i)
		{
		case 0: m_plt->SetVectorType(v.toInt()); break;
		case 1: m_plt->GetColorMap()->SetColorMap(v.toInt()); m_plt->UpdateParticleColors(); break;
		case 2: m_plt->AllowClipping(v.toBool()); break;
		case 3: m_plt->SetSeedTime(v.toInt() - 1); break;
		case 4: m_plt->SetThreshold(v.toFloat()); break;
		case 5: m_plt->SetDensity(v.toFloat()); break;
		case 6: m_plt->SetStepSize(v.toFloat()); break;
		case 7: m_plt->ShowPath(v.toBool()); break;
		}
	}

private:
	CGLParticleFlowPlot*	m_plt;
};

CGLParticleFlowPlot::CGLParticleFlowPlot(CGLModel* mdl) : CGLPlot(mdl), m_find(*mdl->GetActiveMesh())
{
	static int n = 1;
	char szname[128] = { 0 };
	sprintf(szname, "ParticleFlow.%02d", n++);
	SetName(szname);

	m_nvec = -1;
	m_showPath = false;
	m_vtol = 1e-5f;
	m_density = 1.f;

	m_maxtime = -1;
	m_seedTime = 0;
	m_dt = 0.01f;
}

CPropertyList* CGLParticleFlowPlot::propertyList()
{
	return new CGLParticleFlowPlotProps(this);
}

void CGLParticleFlowPlot::SetVectorType(int ntype)
{
	m_nvec = ntype;
	Update(GetModel()->currentTimeIndex(), 0.0, true);
}

void CGLParticleFlowPlot::SetStepSize(float v)
{
	m_dt = v; 
	Update(GetModel()->currentTimeIndex(), 0.0, true);
}

void CGLParticleFlowPlot::SetSeedTime(int n)
{
	if (n < 0) n = 0;
	m_seedTime = n;
	Update(GetModel()->currentTimeIndex(), 0.0, true);
}

void CGLParticleFlowPlot::SetThreshold(float v)
{
	m_vtol = v;
	Update(GetModel()->currentTimeIndex(), 0.0, true);
}

void CGLParticleFlowPlot::SetDensity(float v)
{
	m_density = v;
	Update(GetModel()->currentTimeIndex(), 0.0, true);
}

void CGLParticleFlowPlot::Render(CGLContext& rc)
{
	int NP = (int) m_particles.size();
	if (NP == 0) return;

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);

	glBegin(GL_POINTS);
	for (int i=0; i<NP; ++i)
	{
		FlowParticle& p = m_particles[i];
		if (p.m_balive)
		{
			glColor3ub(p.m_col.r, p.m_col.g, p.m_col.b);
			glVertex3f(p.m_r.x, p.m_r.y, p.m_r.z);
		}
	}
	glEnd();

	if (m_showPath)
	{
		int ntime = GetModel()->currentTimeIndex();
		if (ntime >= m_seedTime + 1)
		{
			glColor3ub(0,0,255);
			for (int i = 0; i<NP; ++i)
			{
				FlowParticle& p = m_particles[i];

				int tend = ntime;
				if (tend > p.m_ndeath) tend = p.m_ndeath;

				glBegin(GL_LINE_STRIP);
				{
					for (int n=m_seedTime; n<=tend; ++n)
					{
						vec3f& r = p.m_pos[n];
						glVertex3f(r.x, r.y, r.z);
					}					
				}
				glEnd();
			}
		}
	}

	glPopAttrib();
}

void CGLParticleFlowPlot::Update(int ntime, float dt, bool breset)
{
	if (breset) { m_map.Clear(); m_rng.clear(); m_maxtime = -1; m_particles.clear(); }
	if (m_nvec == -1) return;
	
	CGLModel* mdl = GetModel();

	FEMeshBase* pm = mdl->GetActiveMesh();
	FEModel* pfem = mdl->GetFEModel();

	if (m_map.States() == 0)
	{
		int NS = pfem->GetStates();
		int NN = pm->Nodes();

		m_map.Create(NS, NN, vec3f(0, 0, 0), -1);
		m_rng.resize(NS);
	}

	// check the tag
	int ntag = m_map.GetTag(ntime);

	// see if we need to update
	if (ntag != m_nvec)
	{
		// check all states up until this time
		for (int n=0; n<=ntime; ++n)
		{
			ntag = m_map.GetTag(n);
			if (ntag != m_nvec)
			{
				m_map.SetTag(n, m_nvec);

				// get the state we are interested in
				vector<vec3f>& val = m_map.State(n);

				vec2f& rng = m_rng[n];
				rng.x = rng.y = 0;

				float L;

				for (int i = 0; i<pm->Nodes(); ++i)
				{
					val[i] = pfem->EvaluateNodeVector(i, n, m_nvec);
					L = val[i].Length();
					if (L > rng.y) rng.y = L;
				}

				if (rng.y == rng.x) ++rng.y;
			}
		}
	}

	// copy nodal values
	m_crng = m_rng[ntime];

	// update particles
	UpdateParticles(ntime);
}

void CGLParticleFlowPlot::UpdateParticles(int ntime)
{
	if (ntime < m_seedTime)
	{
		// deactivate all particles
		for (int i=0; i<m_particles.size(); ++i) m_particles[i].m_balive = false;
		return;
	}

	if (ntime > m_maxtime)
	{
		// perform particle integration
		if (m_maxtime < m_seedTime)
		{
			// seed the particles
			SeedParticles();
			m_maxtime = m_seedTime;
		}

		// advance the particles from maxtime to this time
		AdvanceParticles(m_maxtime, ntime);
		m_maxtime = ntime;
	}

	// update current state of the particles
	UpdateParticleState(ntime);
}

void CGLParticleFlowPlot::UpdateParticleState(int ntime)
{
	int NP = (int) m_particles.size();
	for (int i=0; i<NP; ++i)
	{
		FlowParticle& p = m_particles[i];

		if (ntime >= p.m_ndeath)
		{
			p.m_balive = false;
		}
		else
		{
			p.m_balive = true;
			p.m_r = p.m_pos[ntime];
			p.m_v = p.m_vel[ntime];
		}
	}

	UpdateParticleColors();
}

void CGLParticleFlowPlot::UpdateParticleColors()
{
	float vmin = m_crng.x;
	float vmax = m_crng.y;
	if (vmax == vmin) vmax++;

	int ncol = m_Col.GetColorMap();
	CColorMap& col = ColorMapManager::GetColorMap(ncol);

	int NP = (int)m_particles.size();
	for (int i = 0; i<NP; ++i)
	{
		FlowParticle& p = m_particles[i];
		if (p.m_balive)
		{
			vec3f& v = p.m_v;
			float V = v.Length();
			float w = (V - vmin) / (vmax - vmin);
			p.m_col = col.map(w);
		}
	}
}

vec3f CGLParticleFlowPlot::Velocity(const vec3f& r, int ntime, float w, bool& ok)
{
	vec3f v(0.f, 0.f, 0.f);
	vec3f ve0[FEGenericElement::MAX_NODES];
	vec3f ve1[FEGenericElement::MAX_NODES];
	FEMeshBase& mesh = *GetModel()->GetActiveMesh();

	vector<vec3f>& val0 = m_map.State(ntime    );
	vector<vec3f>& val1 = m_map.State(ntime + 1);

	int nelem;
	double q[3];
	if (m_find.FindInReferenceFrame(r, nelem, q))
	{
		ok = true;
		FEElement& el = mesh.Element(nelem);

		int ne = el.Nodes();
		for (int i = 0; i<ne; ++i)
		{
			ve0[i] = val0[el.m_node[i]];
			ve1[i] = val1[el.m_node[i]];
		}

		vec3f v0 = el.eval(ve0, q[0], q[1], q[2]);
		vec3f v1 = el.eval(ve1, q[0], q[1], q[2]);

		v = v0*(1.f - w) + v1*w;
	}
	else ok = false;

	return v;
}

void CGLParticleFlowPlot::AdvanceParticles(int n0, int n1)
{
	// get the model
	CGLModel* mdl = GetModel();
	if (mdl == 0) return;
	FEModel& fem = *mdl->GetFEModel();

	// get the mesh
	FEMeshBase& mesh = *mdl->GetActiveMesh();

	BOUNDINGBOX box = m_find.BoundingBox();
	float R = box.GetMaxExtent();
	float dt = m_dt;
	if (dt <= 0.f) return;

	for (int ntime=n0; ntime<n1; ++ntime)
	{
		float t0 = fem.GetState(ntime    )->m_time;
		float t1 = fem.GetState(ntime + 1)->m_time;
		if (t1 < t0) t1 = t0;

		int NP = (int)m_particles.size();
		for (int i = 0; i<NP; ++i)
		{
			FlowParticle& p = m_particles[i];
			p.m_pos[ntime+1] = p.m_pos[ntime];
			p.m_vel[ntime+1] = p.m_vel[ntime];
		}

		float t = t0;
		while (t < t1)
		{
			t += dt;
			if (t > t1) t = t1;
			float w = (t - t0) / (t1 - t0);

			int NP = (int) m_particles.size();
			for (int i=0; i<NP; ++i)
			{
				FlowParticle& p = m_particles[i];

				if (p.m_ndeath > ntime)
				{
					vec3f r0 = p.m_pos[ntime + 1];
					vec3f v0 = p.m_vel[ntime + 1];

					vec3f r1 = r0 + v0*dt;

					bool ok = true;
					vec3f v1 = Velocity(r1, ntime, w, ok);
					if (ok == false)
					{
						p.m_ndeath = ntime + 1;
					}
					else
					{
						p.m_pos[ntime + 1] = r1;
						p.m_vel[ntime + 1] = v1;
					}
				}
			}
		}
	}
}

float frand()
{
	return rand() / (float) RAND_MAX;
}

void CGLParticleFlowPlot::SeedParticles()
{
	// clear current particles, if any
	m_particles.clear();

	// get the model
	CGLModel* mdl = GetModel();
	if (mdl == 0) return;

	// get the number of states
	FEModel* fem = mdl->GetFEModel();
	int NS = fem->GetStates();

	// make sure there is a valid seed time
	if ((m_seedTime < 0) || (m_seedTime >= NS)) return;

	// get the mesh
	FEMeshBase& mesh = *mdl->GetActiveMesh();

	vector<vec3f>& val = m_map.State(m_seedTime);

	// make sure vtol is positive
	float vtol = fabs(m_vtol);

	// loop over all the surface facts
	int NF = mesh.Faces();
	for (int i = 0; i<NF; ++i)
	{
		FEFace& f = mesh.Face(i);

		// evaluate the average velocity at this face
		int nf = f.Nodes();
		vec3f vf(0.f, 0.f, 0.f);
		for (int j = 0; j<nf; ++j) vf += val[f.node[j]];
		vf /= nf;

		// generate random number
		float w = frand();

		// see if this is a valid candidate for a seed
		vec3f fn = f.m_fn;
		if ((fn*vf < -vtol) && (w <= m_density))
		{
			// calculate the face center, this will be the seed
			// NOTE: We are using reference coordinates, therefore we assume that the mesh is not deforming!!
			vec3f cf(0.f, 0.f, 0.f);
			for (int j = 0; j<nf; ++j) cf += mesh.Node(f.node[j]).m_r0;
			cf /= nf;

			// create a particle here
			FlowParticle p;
			p.m_pos.resize(NS);
			p.m_vel.resize(NS);
			p.m_balive = true;
			p.m_ndeath = NS;	// assume the particle will live the entire time

			// set initial position and velocity
			p.m_pos[m_seedTime] = cf;
			p.m_vel[m_seedTime] = vf;

			// add it to the pile
			m_particles.push_back(p);
		}
	}
}
