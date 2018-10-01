#include "stdafx.h"
#include "FEStrainMap.h"
#include "FEMeshData_T.h"

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
		vector<float> D1(m_front1.Nodes());
		for (int i = 0; i<m_front1.Nodes(); ++i)
		{
			int inode = m_front1.m_node[i];
			FENode& node = mesh.Node(inode);
			vec3f r = fem.NodePosition(inode, n);
			vec3f q = project(m_front2, r, n);
			D1[i] = (q - r).Length();
			double s = (q - r)*m_front1.m_norm[i];
			if (s < 0) D1[i] = -D1[i];
		}

		vector<float> L1(m_front1.Nodes());
		for (int i = 0; i<m_front1.Nodes(); ++i)
		{
			int inode = m_front1.m_node[i];
			if (D1[i] < 0)
			{
				FENode& node = mesh.Node(inode);
				vec3f r = fem.NodePosition(inode, n);
				vec3f q = project(m_back1, r, n);
				L1[i] = (q - r).Length();
				double s = (r - q)*m_front1.m_norm[i];
				if (s < 0) L1[i] = -L1[i];
			}
			else L1[i] = 0.0;
		}

		vector<float> s1(m_front1.Nodes());
		for (int i=0; i<m_front1.Nodes(); ++i)
		{
			if (D1[i] < 0.0)
				s1[i] = 0.5*D1[i] / L1[i];
			else
				s1[i] = 0.0;
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
			vec3f q = project(m_front1, r, n);
			D2[i] = (q - r).Length();
			double s = (q - r)*m_front2.m_norm[i];
			if (s < 0) D2[i] = -D2[i];
		}

		vector<float> L2(m_front2.Nodes());
		for (int i = 0; i<m_front2.Nodes(); ++i)
		{
			int inode = m_front2.m_node[i];
			if (D2[i] < 0)
			{
				FENode& node = mesh.Node(inode);
				vec3f r = fem.NodePosition(inode, n);
				vec3f q = project(m_back2, r, n);
				L2[i] = (q - r).Length();
				double s = (r - q)*m_front2.m_norm[i];
				if (s < 0) L2[i] = -L2[i];
			}
			else L2[i] = 0.0;
		}

		vector<float> s2(m_front2.Nodes());
		for (int i = 0; i<m_front2.Nodes(); ++i)
		{
			if (D2[i] < 0.0)
				s2[i] = 0.5*D2[i] / L2[i];
			else
				s2[i] = 0.0;
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
vec3f FEStrainMap::project(FEStrainMap::Surface& surf, vec3f& r, int ntime)
{
	FEMeshBase& mesh = *m_fem->GetFEMesh(0);

	// find the closest surface node
	vec3f q = m_fem->NodePosition(surf.m_node[0], ntime);
	float Dmin = (q - r)*(q - r);
	int imin = 0;
	for (int i = 1; i<surf.Nodes(); ++i)
	{
		vec3f p = m_fem->NodePosition(surf.m_node[i], ntime);
		float D = (p - r)*(p - r);
		if (D < Dmin)
		{
			q = p;
			Dmin = D;
			imin = i;
		}
	}

	// loop over all facets connected to this node
	vector<int>& FT = surf.m_NLT[imin];
	for (int i = 0; i<(int)FT.size(); ++i)
	{
		// get the i-th facet
		FEFace& face = mesh.Face(FT[i]);

		// project r onto the the facet
		vec3f p;
		if (ProjectToFacet(face, r, ntime, p))
		{
			// return the closest projection
			float D = (p - r)*(p - r);
			if (D < Dmin)
			{
				q = p;
				Dmin = D;
			}
		}
	}

	return q;
}

//-----------------------------------------------------------------------------
bool FEStrainMap::ProjectToFacet(FEFace& f, vec3f& x, int ntime, vec3f& q)
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
	case 3: return ProjectToTriangle(y, x, q); break;
	case 4: return ProjectToQuad(y, x, q); break;
	default:
		assert(false);
	}
	return false;
}


//-----------------------------------------------------------------------------
// project onto a triangular face
bool FEStrainMap::ProjectToTriangle(vec3f* y, vec3f& x, vec3f& q)
{
	// calculate base vectors 
	vec3f e1 = y[1] - y[0];
	vec3f e2 = y[2] - y[0];

	// calculate plane normal
	vec3f n = e1^e2; n.Normalize();

	// project x onto the plane
	q = x - n*((x - y[0])*n);

	// set up metric tensor
	double G[2][2];
	G[0][0] = e1*e1;
	G[0][1] = G[1][0] = e1*e2;
	G[1][1] = e2*e2;

	// invert metric tensor
	double D = G[0][0] * G[1][1] - G[0][1] * G[1][0];
	double Gi[2][2];
	Gi[0][0] = G[1][1] / D;
	Gi[1][1] = G[0][0] / D;
	Gi[0][1] = Gi[1][0] = -G[0][1] / D;

	// calculate dual base vectors
	vec3f E1 = e1*Gi[0][0] + e2*Gi[0][1];
	vec3f E2 = e1*Gi[1][0] + e2*Gi[1][1];

	// now we can calculate r and s
	vec3f t = q - y[0];
	double r = t*E1;
	double s = t*E2;

	return ((r >= -m_tol) && (s >= -m_tol) && (r + s <= 1.0 + m_tol));
}

//-----------------------------------------------------------------------------
// project onto a quadrilateral surface.
bool FEStrainMap::ProjectToQuad(vec3f* y, vec3f& x, vec3f& q)
{
	double R[2], u[2], D;
	double gr[4] = { -1, +1, +1, -1 };
	double gs[4] = { -1, -1, +1, +1 };
	double H[4], Hr[4], Hs[4], Hrs[4];

	int i, j;
	int NMAX = 50, n = 0;

	// evaulate scalar products
	double xy[4] = { x*y[0], x*y[1], x*y[2], x*y[3] };
	double yy[4][4];
	yy[0][0] = y[0] * y[0]; yy[1][1] = y[1] * y[1]; yy[2][2] = y[2] * y[2]; yy[3][3] = y[3] * y[3];
	yy[0][1] = yy[1][0] = y[0] * y[1];
	yy[0][2] = yy[2][0] = y[0] * y[2];
	yy[0][3] = yy[3][0] = y[0] * y[3];
	yy[1][2] = yy[2][1] = y[1] * y[2];
	yy[1][3] = yy[3][1] = y[1] * y[3];
	yy[2][3] = yy[3][2] = y[2] * y[3];

	double r = 0, s = 0;

	// loop until converged
	bool bconv = false;
	double normu;
	do
	{
		// evaluate shape functions and shape function derivatives.
		for (i = 0; i<4; ++i)
		{
			H[i] = 0.25*(1 + gr[i] * r)*(1 + gs[i] * s);

			Hr[i] = 0.25*gr[i] * (1 + gs[i] * s);
			Hs[i] = 0.25*gs[i] * (1 + gr[i] * r);

			Hrs[i] = 0.25*gr[i] * gs[i];
		}

		// set up the system of equations
		R[0] = R[1] = 0;
		double A[2][2] = { 0 };
		for (i = 0; i<4; ++i)
		{
			R[0] -= (xy[i])*Hr[i];
			R[1] -= (xy[i])*Hs[i];

			A[0][1] += (xy[i])*Hrs[i];
			A[1][0] += (xy[i])*Hrs[i];

			for (j = 0; j<4; ++j)
			{
				double yij = yy[i][j];
				R[0] -= -H[j] * Hr[i] * (yij);
				R[1] -= -H[j] * Hs[i] * (yij);

				A[0][0] -= (yij)*(Hr[i] * Hr[j]);
				A[1][1] -= (yij)*(Hs[i] * Hs[j]);

				A[0][1] -= (yij)*(Hr[i] * Hs[j] + Hrs[i] * H[j]);
				A[1][0] -= (yij)*(Hs[i] * Hr[j] + Hrs[i] * H[j]);
			}
		}

		// determinant of A
		D = A[0][0] * A[1][1] - A[0][1] * A[1][0];

		// solve for u = A^(-1)*R
		u[0] = (A[1][1] * R[0] - A[0][1] * R[1]) / D;
		u[1] = (A[0][0] * R[1] - A[1][0] * R[0]) / D;

		// calculate displacement norm
		normu = u[0] * u[0] + u[1] * u[1];

		// check for convergence
		bconv = ((normu < 1e-10));
		if (!bconv && (n <= NMAX))
		{
			// Don't update if converged otherwise the point q
			// does not correspond with the current values for (r,s)
			r += u[0];
			s += u[1];
			++n;
		}
		else break;
	} while (1);

	// evaluate q
	q = y[0] * H[0] + y[1] * H[1] + y[2] * H[2] + y[3] * H[3];

	return ((r >= -1.0 - m_tol) && (r <= 1.0 + m_tol) && (s >= -1.0 - m_tol) && (s <= 1.0 + m_tol));
}
