#include "stdafx.h"
#include "FEStrainMap.h"
#include "FEMeshData_T.h"
#include "tools.h"

//-----------------------------------------------------------------------------
void FEStrainMap::Surface::BuildNodeList(FEMeshBase& mesh)
{
	// tag all nodes that belong to this surface
	int N = mesh.Nodes();
	for (int i = 0; i<N; ++i) mesh.Node(i).m_ntag = -1;
	int nn = 0;
	for (int i = 0; i<Faces(); ++i)
	{
		FEFace& f = mesh.Face(m_face[i]);
		int nf = f.Nodes();
		for (int j = 0; j<nf; ++j)
		{
			FENode& node = mesh.Node(f.node[j]);
			if (node.m_ntag == -1) node.m_ntag = nn++;
		}
	}

	// create the global node list
	m_node.resize(nn);
	for (int i = 0; i<N; ++i)
	{
		FENode& node = mesh.Node(i);
		if (node.m_ntag >= 0) m_node[node.m_ntag] = i;
	}

	// create the local node list
	m_lnode.resize(Faces() * 4);
	for (int i = 0; i<Faces(); ++i)
	{
		FEFace& f = mesh.Face(m_face[i]);
		if (f.Nodes() == 4)
		{
			m_lnode[4 * i] = mesh.Node(f.node[0]).m_ntag; assert(m_lnode[4 * i] >= 0);
			m_lnode[4 * i + 1] = mesh.Node(f.node[1]).m_ntag; assert(m_lnode[4 * i + 1] >= 0);
			m_lnode[4 * i + 2] = mesh.Node(f.node[2]).m_ntag; assert(m_lnode[4 * i + 2] >= 0);
			m_lnode[4 * i + 3] = mesh.Node(f.node[3]).m_ntag; assert(m_lnode[4 * i + 3] >= 0);
		}
		else if (f.Nodes() == 3)
		{
			m_lnode[4 * i] = mesh.Node(f.node[0]).m_ntag; assert(m_lnode[4 * i] >= 0);
			m_lnode[4 * i + 1] = mesh.Node(f.node[1]).m_ntag; assert(m_lnode[4 * i + 1] >= 0);
			m_lnode[4 * i + 2] = mesh.Node(f.node[2]).m_ntag; assert(m_lnode[4 * i + 2] >= 0);
			m_lnode[4 * i + 3] = m_lnode[4 * i + 2];
		}
		else assert(false);
	}

	// create the node-facet look-up table
	m_NLT.resize(Nodes());
	for (int i = 0; i<Faces(); ++i)
	{
		FEFace& f = mesh.Face(m_face[i]);
		int nf = f.Nodes();
		for (int j = 0; j<nf; ++j)
		{
			int inode = m_lnode[4 * i + j];
			m_NLT[inode].push_back(m_face[i]);
		}
	}
}


// constructor
FEStrainMap::FEStrainMap()
{
	m_tol = 0.01;
}

// assign selections
void FEStrainMap::SetFrontSurface1(std::vector<int>& s)
{
	m_front1.m_face = s;
}

void FEStrainMap::SetBackSurface1(std::vector<int>& s)
{
	m_back1.m_face = s;
}

void FEStrainMap::SetFrontSurface2(std::vector<int>& s)
{
	m_front2.m_face = s;
}

void FEStrainMap::SetBackSurface2(std::vector<int>& s)
{
	m_back2.m_face = s;
}

// apply the map
void FEStrainMap::Apply(FEModel& fem)
{
	static int ncalls = 0; ncalls++;
	char szname[64];
	if (ncalls == 1)
		sprintf(szname, "strain map");
	else
		sprintf(szname, "strain map (%d)", ncalls);

	// store the model
	m_fem = &fem;

	// add a new data field
	fem.AddDataField(new FEDataField_T<FEFaceData<float, DATA_NODE> >(szname, EXPORT_DATA));
	int NDATA = fem.GetDataManager()->DataFields() - 1;

	// get the mesh
	FEMeshBase& mesh = *fem.GetFEMesh(0);

	// build the node lists
	m_front1.BuildNodeList(mesh);
	m_back1.BuildNodeList(mesh);
	m_front2.BuildNodeList(mesh);
	m_back2.BuildNodeList(mesh);
	int N = mesh.Nodes();

	BuildNormalList(m_front1);
	BuildNormalList(m_back1);
	BuildNormalList(m_front2);
	BuildNormalList(m_back2);

	// repeat for all steps
	int nstep = fem.GetStates();
	for (int n = 0; n<nstep; ++n)
	{
		FEState* ps = fem.GetState(n);
		FEFaceData<float, DATA_NODE>& df = dynamic_cast<FEFaceData<float, DATA_NODE>&>(ps->m_Data[NDATA]);

		// loop over all nodes of surface 1
		vector<float> D1(m_front1.Nodes(), 0.f);
		vec3f q;
		for (int i = 0; i<m_front1.Nodes(); ++i)
		{
			int inode = m_front1.m_node[i];
			FENode& node = mesh.Node(inode);
			vec3f r = fem.NodePosition(inode, n);
			if (project(m_front2, r, m_front1.m_norm[i], n, q))
			{
				D1[i] = (q - r).Length();
				double s = (q - r)*m_front1.m_norm[i];
				if (s < 0) D1[i] = -D1[i];
			}
		}

		vector<float> L1(m_front1.Nodes());
		for (int i = 0; i<m_front1.Nodes(); ++i)
		{
			int inode = m_front1.m_node[i];
			if (D1[i] < 0)
			{
				FENode& node = mesh.Node(inode);
				vec3f r = fem.NodePosition(inode, n);
				if (project(m_back1, r, m_front1.m_norm[i], n, q))
				{
					L1[i] = (q - r).Length();
					double s = (r - q)*m_front1.m_norm[i];
					if (s < 0) L1[i] = -L1[i];
				}
				else L1[i] = 1e+34f;	// really large number, such that the strain is zero
			}
			else L1[i] = 0.0;
		}

		vector<float> s1(m_front1.Nodes());
		for (int i=0; i<m_front1.Nodes(); ++i)
		{
			if (D1[i] < 0.0)
				s1[i] = 0.5f*D1[i] / L1[i];
			else
				s1[i] = 0.f;
		}

		vector<int> nf1(m_front1.Faces());
		for (int i = 0; i<m_front1.Faces(); ++i) nf1[i] = 4; //mesh.Face(m_surf1.m_face[i]).Nodes();
		df.add(s1, m_front1.m_face, m_front1.m_lnode, nf1);

		// loop over all nodes of surface 2
		vector<float> D2(m_front2.Nodes());
		for (int i = 0; i<m_front2.Nodes(); ++i)
		{
			int inode = m_front2.m_node[i];
			FENode& node = mesh.Node(inode);
			vec3f r = fem.NodePosition(inode, n);
			if (project(m_front1, r, m_front2.m_norm[i], n, q))
			{
				D2[i] = (q - r).Length();
				double s = (q - r)*m_front2.m_norm[i];
				if (s < 0) D2[i] = -D2[i];
			}
			else D2[i] = 0.f;
		}

		vector<float> L2(m_front2.Nodes());
		for (int i = 0; i<m_front2.Nodes(); ++i)
		{
			int inode = m_front2.m_node[i];
			if (D2[i] < 0)
			{
				FENode& node = mesh.Node(inode);
				vec3f r = fem.NodePosition(inode, n);
				if (project(m_back2, r, m_front2.m_norm[i], n, q))
				{
					L2[i] = (q - r).Length();
					double s = (r - q)*m_front2.m_norm[i];
					if (s < 0) L2[i] = -L2[i];
				}
				else L2[i] = 1e+34f;	// really large number, such that the strain is zero
			}
			else L2[i] = 0.f;
		}

		vector<float> s2(m_front2.Nodes());
		for (int i = 0; i<m_front2.Nodes(); ++i)
		{
			if (D2[i] < 0.0)
				s2[i] = 0.5f*D2[i] / L2[i];
			else
				s2[i] = 0.f;
		}

		vector<int> nf2(m_front2.Faces());
		for (int i = 0; i<m_front2.Faces(); ++i) nf2[i] = 4; //mesh.Face(m_surf2.m_face[i]).Nodes();
		df.add(s2, m_front2.m_face, m_front2.m_lnode, nf2);
	}
}

//-----------------------------------------------------------------------------
void FEStrainMap::BuildNormalList(FEStrainMap::Surface& s)
{
	// get the mesh
	FEMeshBase& mesh = *m_fem->GetFEMesh(0);

	int NF = s.Faces();
	int NN = s.Nodes();
	s.m_norm.resize(NN);

	for (int i = 0; i<NF; ++i)
	{
		FEFace& f = mesh.Face(s.m_face[i]);
		int nf = f.Nodes();
		for (int j = 0; j<nf; ++j)
		{
			int n = s.m_lnode[4 * i + j]; assert(n >= 0);
			s.m_norm[n] = f.m_nn[j];
		}
	}
}

//-----------------------------------------------------------------------------
bool FEStrainMap::project(FEStrainMap::Surface& surf, vec3f& r, vec3f& t, int ntime, vec3f& q)
{
	FEMeshBase& mesh = *m_fem->GetFEMesh(0);

	// loop over all facets
	float Dmin = 0.f;
	bool bfound = false;
	for (int i = 0; i<surf.Faces(); ++i)
	{
		// get the i-th facet
		FEFace& face = mesh.Face(surf.m_face[i]);

		// project r onto the the facet along its normal
		vec3f p;
		if (ProjectToFacet(face, r, t, ntime, p))
		{
			// return the closest projection
			float D = (p - r)*(p - r);
			if ((D < Dmin) || (bfound == false))
			{
				q = p;
				Dmin = D;
				bfound = true;
			}
		}
	}

	return bfound;
}

//-----------------------------------------------------------------------------
bool FEStrainMap::ProjectToFacet(FEFace& f, vec3f& x, vec3f& t, int ntime, vec3f& q)
{
	// get the mesh to which this surface belongs
	FEMeshBase& mesh = *m_fem->GetFEMesh(0);

	// number of element nodes
	int ne = f.Nodes();

	// get the elements nodal positions
	vec3f y[4];
	for (int i = 0; i<ne; ++i) y[i] = m_fem->NodePosition(f.node[i], ntime);

	// calculate normal projection of x onto element
	switch (ne)
	{
	case 3: return ProjectToTriangle(y, x, t, q, m_tol); break;
	case 4: return ProjectToQuad(y, x, t, q, m_tol); break;
	default:
		assert(false);
	}
	return false;
}
