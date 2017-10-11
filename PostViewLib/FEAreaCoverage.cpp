#include "stdafx.h"
#include "FEAreaCoverage.h"
#include "FEModel.h"
#include "FEMeshData_T.h"
#include "Intersect.h"


//-----------------------------------------------------------------------------
void FEAreaCoverage::Surface::BuildNodeList(FEMeshBase& mesh)
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


//-----------------------------------------------------------------------------
FEAreaCoverage::FEAreaCoverage() : m_fem(0)
{
}

//-----------------------------------------------------------------------------
void FEAreaCoverage::Apply(FEModel& fem)
{
	static int ncalls = 0; ncalls++;

	if (m_name.empty())
	{
		char szname[64];
		if (ncalls == 1)
			sprintf(szname, "area coverage");
		else
			sprintf(szname, "area coverage (%d)", ncalls);

		m_name = szname;
	}

	// store the model
	m_fem = &fem;

	// add a new data field
	fem.AddDataField(new FEDataField_T<FEFaceData<float, DATA_NODE> >(m_name.c_str(), EXPORT_DATA));
	int NDATA = fem.GetDataManager()->DataFields() - 1;

	// get the mesh
	FEMeshBase& mesh = *fem.GetFEMesh(0);

	// build the node lists
	m_surf1.BuildNodeList(mesh);
	m_surf2.BuildNodeList(mesh);

	// build the normal lists
	BuildNormalList(m_surf1);
	BuildNormalList(m_surf2);

	// repeat for all steps
	int nstep = fem.GetStates();
	for (int n = 0; n<nstep; ++n)
	{
		FEState* ps = fem.GetState(n);
		FEFaceData<float, DATA_NODE>& df = dynamic_cast<FEFaceData<float, DATA_NODE>&>(ps->m_Data[NDATA]);

		// repeat over all nodes of surface 1
		vector<float> a(m_surf1.Nodes(), 0.f);
		for (int i = 0; i<m_surf1.Nodes(); ++i)
		{
			int inode = m_surf1.m_node[i];
			FENode& node = mesh.Node(inode);
			vec3f ri = fem.NodePosition(inode, n);
			vec3f Ni = m_surf1.m_norm[i];

			// see if it intersects the other surface
			if (intersect(n, ri, Ni, m_surf2))
			{
				a[i] = 1.f;
			}
		}
		vector<int> nf1(m_surf1.Faces());
		for (int i = 0; i<m_surf1.Faces(); ++i) nf1[i] = mesh.Face(m_surf1.m_face[i]).Nodes();
		df.add(a, m_surf1.m_face, m_surf1.m_lnode, nf1);


		// repeat over all nodes of surface 2
		vector<float> b(m_surf2.Nodes(), 0.f);
		for (int i = 0; i<m_surf2.Nodes(); ++i)
		{
			int inode = m_surf2.m_node[i];
			FENode& node = mesh.Node(inode);
			vec3f ri = fem.NodePosition(inode, n);
			vec3f Ni = m_surf2.m_norm[i];

			// see if it intersects the other surface
			if (intersect(n, ri, Ni, m_surf1))
			{
				b[i] = 1.f;
			}
		}
		vector<int> nf2(m_surf2.Faces());
		for (int i = 0; i<m_surf2.Faces(); ++i) nf2[i] = mesh.Face(m_surf2.m_face[i]).Nodes();
		df.add(b, m_surf2.m_face, m_surf2.m_lnode, nf2);
	}
}

//-----------------------------------------------------------------------------
void FEAreaCoverage::BuildNormalList(FEAreaCoverage::Surface& s)
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
bool FEAreaCoverage::intersect(int nstate, const vec3f& r, const vec3f& N, FEAreaCoverage::Surface& surf)
{
	FEMeshBase& mesh = *m_fem->GetFEMesh(0);

	// create the ray
	Ray ray = {r, N};

	// loop over all facets connected to this node
	Intersection q;
	for (int i = 0; i<(int)surf.m_face.size(); ++i)
	{
		// get the i-th facet
		FEFace& face = mesh.Face(surf.m_face[i]);

		// see if the ray intersects this face
		if (faceIntersect(nstate, ray, face))
		{
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------
bool FEAreaCoverage::faceIntersect(int nstate, const Ray& ray, const FEFace& face)
{
	Intersection q;
	q.m_index = -1;

	vec3f rn[10];
	for (int i = 0; i<face.Nodes(); ++i)
	{
		rn[i] = m_fem->NodePosition(face.node[i], nstate);
	}

	bool bfound = false;
	switch (face.m_ntype)
	{
	case FACE_TRI3:
	case FACE_TRI6:
	case FACE_TRI7:
	case FACE_TRI10:
	{
		Triangle tri = { rn[0], rn[1], rn[2] };
		bfound = IntersectTriangle(ray, tri, q);
	}
	break;
	case FACE_QUAD4:
	case FACE_QUAD8:
	case FACE_QUAD9:
	{
		Quad quad = { rn[0], rn[1], rn[2], rn[3] };
		bfound = FastIntersectQuad(ray, quad, q);
	}
	break;
	}

	return bfound;
}
