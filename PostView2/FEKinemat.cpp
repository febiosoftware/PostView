#include "stdafx.h"
#include "FEKinemat.h"
#include "Document.h"
#include "PostViewLib/FELSDYNAimport.h"
#include "PostViewLib/FEMeshData_T.h"

//-----------------------------------------------------------------------------
vec3f FEKinemat::KINE::apply(vec3f& r)
{
	float q[3] = {r.x, r.y, r.z};
	float d[3];
	d[0] = m[0]*q[0] + m[1]*q[1] + m[ 2]*q[2] + m[ 3];
	d[1] = m[4]*q[0] + m[5]*q[1] + m[ 6]*q[2] + m[ 7];
	d[2] = m[8]*q[0] + m[9]*q[1] + m[10]*q[2] + m[11];
	return vec3f(d[0], d[1], d[2]);
}

//-----------------------------------------------------------------------------
FEKinemat::FEKinemat(CDocument* pdoc) : m_pDoc(pdoc)
{

}

//-----------------------------------------------------------------------------
void FEKinemat::SetRange(int n0, int n1, int ni)
{
	if (n1 < n0) n1 = n0;
	if (ni < 0) ni = 1;
	m_n0 = n0 - 1;
	m_n1 = n1 - 1;
	m_ni = ni;
}

//-----------------------------------------------------------------------------
bool FEKinemat::Apply(const char* szfile, const char* szkine)
{
	// load the file
	FELSDYNAimport* preader = new FELSDYNAimport;
	preader->read_displacements(true);
	if (m_pDoc->LoadFEModel(preader, szfile) == false) return false;

	// read the kinematics file data
	if (ReadKine(szkine) == false) return false;

	// build the states
	if (BuildStates() == false) return false;

	return true;
}

//-----------------------------------------------------------------------------
bool FEKinemat::ReadKine(const char* szfile)
{
	FILE* fp = fopen(szfile, "rt");
	if (fp == 0) return false;

	STATE s; KINE d;
	char szline[512] = {0};
	while (fgets(szline, 511, fp))
	{
		char* ch = szline;
		int n = 0;
		s.D.clear();
		do
		{
			d.m[n++] = (float) atof(ch);
			if (n == 16)
			{
				s.D.push_back(d);
				n = 0;
			}
			ch = strchr(ch, ',');
			if (ch) ch++;
		}
		while (ch);

		m_State.push_back(s);
	}

	fclose(fp);
	return true;
}

//-----------------------------------------------------------------------------
bool FEKinemat::BuildStates()
{
	FEModel& fem = *m_pDoc->GetFEModel();
	FEMesh& mesh = *fem.GetMesh();
	int NMAT = fem.Materials();
	int NN = mesh.Nodes();
	int NE = mesh.Elements();

	// find the displacement field
	FEDataManager* pdm = fem.GetDataManager();
	int N = pdm->DataFields();
	FEDataFieldPtr pt = pdm->FirstDataField();
	int ND = -1;
	for (int i=0; i<N; ++i, ++pt)
	{
		 if (strcmp((*pt)->GetName(), "Displacement") == 0)
		 {
			 ND = i;
			 break;
		 }
	}
	if (ND == -1) return false;

	int NS = m_State.size();
	if (m_n0 >= NS) return false;
	if (m_n1 - m_n0 +1 > NS) m_n1 = NS - 1;
	float t = 0.f;
	for (int ns = m_n0; ns <= m_n1; ns += m_ni, t += 1.f)
	{
		STATE& s = m_State[ns];

		// create a new state
		FEState* ps = 0;
		if (t == 0.f) ps = fem.GetState(0);
		else {
			try {
				ps = new FEState(t, &fem);
				fem.AddState(ps);
			}
			catch (...)
			{
				return false;
			}
		}

		// get the displacement field
		FENodeData<vec3f>& d = dynamic_cast<FENodeData<vec3f>&>(ps->m_Data[ND]);

		for (int i=0; i<NN; ++i) d[i] = vec3f(0.f, 0.f, 0.f);

		int N = NMAT;
		if (s.D.size() < NMAT) N = s.D.size();
		for (int n=0; n<N; ++n)
		{
			KINE& kine = s.D[n];
			for (int i=0; i<NN; ++i) mesh.Node(i).m_ntag = 0;
			for (int i=0; i<NE; ++i)
			{
				FEElement& e = mesh.Element(i);
				if (e.m_MatID == n)
				{
					int ne = e.Nodes();
					for (int j=0; j<ne; ++j) mesh.Node(e.m_node[j]).m_ntag = 1;
				}
			}

			for (int i=0; i<NN; ++i)
			{
				FENode& nd = mesh.Node(i);
				if (nd.m_ntag == 1)
				{
					vec3f& rt = nd.m_rt;
					vec3f& r0 = nd.m_r0;
					if (t == 0.f)
					{
						 r0 = kine.apply(rt);
						 d[i] = vec3f(0.f, 0.f, 0.f);
					}
					else d[i] = kine.apply(rt) - r0;
				}
			}
		}
	}
	fem.UpdateBoundingBox();
	m_pDoc->ResetView();
	return true;
}
