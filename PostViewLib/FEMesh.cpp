// FEMesh.cpp: implementation of the FEMesh class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEMesh.h"
#include <stack>

//=============================================================================

//-----------------------------------------------------------------------------
// Constructor
FEMesh::FEMesh()
{
	m_stol = 60.*PI/180.0;
	NewID();
}

//-----------------------------------------------------------------------------
// Destructor
FEMesh::~FEMesh()
{
	CleanUp();
}

//-----------------------------------------------------------------------------
// Generate a unique mesh-ID (this is used in the Document Update to see if 
// the mesh has to be recreated)
void FEMesh::NewID()
{
	static int n = 1;
	m_nID = n++;
}

//-----------------------------------------------------------------------------
// clean all depdendant structures
void FEMesh::CleanUp()
{
	m_NEL.Clear();
	m_NFL.Clear();

	ClearDomains();
	ClearParts();
	ClearSurfaces();
	ClearNodeSets();
}

//-----------------------------------------------------------------------------
// clear everything
void FEMesh::ClearAll()
{
	CleanUp();
	m_Node.clear();
	m_Face.clear();
	m_Elem.clear();
}

//-----------------------------------------------------------------------------
// Clear all the domains
void FEMesh::ClearDomains()
{
	for (int i=0; i<(int) m_Dom.size(); ++i) delete m_Dom[i];
	m_Dom.clear();
}

//-----------------------------------------------------------------------------
// Clear all the parts
void FEMesh::ClearParts()
{
	for (int i=0; i<(int) m_Part.size(); ++i) delete m_Part[i];
	m_Part.clear();
}

//-----------------------------------------------------------------------------
// Clear all the surfaces
void FEMesh::ClearSurfaces()
{
	for (int i=0; i<(int) m_Surf.size(); ++i) delete m_Surf[i];
	m_Surf.clear();
}

//-----------------------------------------------------------------------------
// Clear all the node sets
void FEMesh::ClearNodeSets()
{
	for (int i=0; i<(int) m_NSet.size(); ++i) delete m_NSet[i];
	m_NSet.clear();
}

//-----------------------------------------------------------------------------
// Count nr of beam elements
int FEMesh::BeamElements()
{
	int n = 0;
	for (int i=0; i<Elements(); ++i)
	{
		if (m_Elem[i].IsBeam()) n++;
	}

	return n;
}

//-----------------------------------------------------------------------------
// Count nr of shell elements
int FEMesh::ShellElements()
{
	int n = 0;
	for (int i=0; i<Elements(); ++i)
	{
		if (m_Elem[i].IsShell()) n++;
	}

	return n;
}

//-----------------------------------------------------------------------------
// Count nr of solid elements
int FEMesh::SolidElements()
{
	int n = 0;
	for (int i=0; i<Elements(); ++i)
	{
		if (m_Elem[i].IsSolid()) n++;
	}

	return n;
}

//-----------------------------------------------------------------------------
// Create a mesh
bool FEMesh::Create(int nodes, int elems)
{
	int i;
	// clean up the old mesh
	CleanUp();

	// allocate storage for nodal data
	if (nodes) m_Node.resize(nodes);

	// allocate storage for element data
	if (elems)
	{
		m_Elem.resize(elems);

		// make sure everything got allocated
		if ((m_Node.size() == 0) || (m_Elem.size() == 0))
		{
			CleanUp();
			return false;
		}

		// set element ID's
		for (i=0; i<elems; i++) m_Elem[i].m_nId = i;	
	}

	return true;
}

//-----------------------------------------------------------------------------
// Find the element neighbours
void FEMesh::FindNeighbours()
{
	int i, j, k, l;

	// Build the node-element list
	m_NEL.Build(this);

	// set up the element's neighbour pointers
	FEFace face;
	FEEdge edge;
	FEElement* pne;
	bool bfound;
	for (i=0; i<Elements(); i++)
	{
		FEElement& e = m_Elem[i];

		// first, do the solid elements
		for (j=0; j<e.Faces(); j++)
		{
			e.m_pElem[j] = 0;
			face = e.GetFace(j);

			// find the neighbour element
			vector<NodeElemRef>& nel = m_NEL.ElemList(face.node[0]);
			bfound = false;
			for (k=0; k < (int) nel.size(); k++)
			{
				pne = &m_Elem[nel[k].first];
				if ((pne != &e) && ( *pne != e))
				{
					for (l=0; l<pne->Faces(); l++) 
					{
						FEFace f2 = pne->GetFace(l);
						if (face == f2)
						{
							bfound = true;
							break;
						}
					}

					if (bfound)
					{
						e.m_pElem[j] = pne;
						break;
					}
				}
			}
		}

		// next, do the shell elements
		for (j=0; j<e.Edges(); ++j)
		{
			e.m_pElem[j] = 0;
			edge = e.GetEdge(j);

			// find the neighbour element
			vector<NodeElemRef>& nel = m_NEL.ElemList(edge.node[0]);
			bfound = false;
			for (k=0; k < (int) nel.size(); k++)
			{
				pne = &m_Elem[nel[k].first];
				if ((pne != &e) && (*pne != e))
				{
					for (l=0; l<pne->Edges(); l++) 
						if (edge == pne->GetEdge(l))
						{
							bfound = true;
							break;
						}

					if (bfound)
					{
						e.m_pElem[j] = pne;
						break;
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Build the parts
void FEMesh::UpdateDomains()
{
	ClearDomains();

	// figure out how many domains there are
	int ndom = 0, i;
	for (i=0; i<(int) m_Elem.size(); ++i) if (m_Elem[i].m_MatID > ndom) ndom = m_Elem[i].m_MatID;
	++ndom;

	m_Dom.resize(ndom);
	for (i=0; i<ndom; ++i) m_Dom[i] = new FEDomain(this, i);
}

//-----------------------------------------------------------------------------
// Build the FE faces. Note that we only create exterior faces
void FEMesh::BuildFaces()
{
	// make sure we only call this once
	assert(m_Face.empty());
	int i, j;

	// let's count the faces
	int NF = 0;
	int NE = m_Elem.size();
	for (i=0; i<NE; ++i)
	{
		FEElement& e = m_Elem[i];

		// solid elements
		int nf = e.Faces();
		for (j=0; j<nf; ++j)
			if (e.m_pElem[j] == 0) ++NF;

		// shell elements
		if (e.Edges()) ++NF;
	}

	// allocate storage
	m_Face.resize(NF);

	// create faces
	NF = 0;
	for (i=0; i<NE; ++i)
	{
		FEElement& e = m_Elem[i];
		
		// solid elements
		int nf = e.Faces();
		for (j=0; j<nf; ++j)
			if (e.m_pElem[j] == 0)
			{
				FEFace& f = m_Face[NF++];
				f = e.GetFace(j);
				f.m_elem[0] = i;
				f.m_elem[1] = j;
				f.m_mat = e.m_MatID;
			}

		// shell elements
		if (e.Edges()>0)
		{
			FEFace& f = m_Face[NF++];
			f.node[0] = e.m_node[0];
			f.node[1] = e.m_node[1];
			f.node[2] = e.m_node[2];
			if (e.m_ntype == FE_QUAD4) 
			{
				f.node[3] = e.m_node[3];
				f.m_ntype = FACE_QUAD4;
			}
            else if (e.m_ntype == FE_QUAD8)
            {
                f.node[3] = e.m_node[3];
                f.node[4] = e.m_node[4];
                f.node[5] = e.m_node[5];
                f.node[6] = e.m_node[6];
                f.node[7] = e.m_node[7];
                f.m_ntype = FACE_QUAD8;
            }
            else if (e.m_ntype == FE_QUAD9)
            {
                f.node[3] = e.m_node[3];
                f.node[4] = e.m_node[4];
                f.node[5] = e.m_node[5];
                f.node[6] = e.m_node[6];
                f.node[7] = e.m_node[7];
                f.node[8] = e.m_node[8];
                f.m_ntype = FACE_QUAD9;
            }
			else if (e.m_ntype == FE_TRI3)
			{
				f.node[3] = e.m_node[2];
				f.m_ntype = FACE_TRI3;
			}
            else if (e.m_ntype == FE_TRI6)
            {
                f.node[3] = e.m_node[3];
                f.node[4] = e.m_node[4];
                f.node[5] = e.m_node[5];
                f.m_ntype = FACE_TRI6;
            }

			f.m_elem[0] = i;
			f.m_elem[1] = 0;
			f.m_mat = e.m_MatID;
		}
	}

	// build the node-face list
	m_NFL.Build(this);
}

//-----------------------------------------------------------------------------
// Update the FE data
void FEMesh::Update()
{
	// find the element's neighbours
	if (m_NEL.Empty()) FindNeighbours();

	// now that we have found the neighbours, let's find the faces
	if (m_Face.empty()) BuildFaces();

	// create the parts
	UpdateDomains();

	// Calculate SG and normals
	AutoSmooth();

	// now we can figure out which nodes are interior and which are exterior
	UpdateNodes();
}

//-----------------------------------------------------------------------------
// Find the interior and exterior nodes
void FEMesh::UpdateNodes()
{
	int i, j;

	int nodes = Nodes();
	int faces = Faces();
	int elems = Elements();

	for (i=0; i<nodes; ++i) 
	{
		FENode& n = m_Node[i];
		n.m_bext = false;
	}
	
	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		int nf = f.Nodes();
		for (j=0; j<nf; ++j) m_Node[f.node[j]].m_bext = true;
	}
}

//-----------------------------------------------------------------------------
// Partition the surface depending on a smoothing tolerance. Face neighbours
// are only set when the faces belong to the same smoothing group.
void FEMesh::AutoSmooth()
{
	int i, j, k, n;

	int nodes = Nodes();
	int faces = Faces();

	// calculate the valences
	int* pnv = new int[nodes];
	for (i=0; i<nodes; ++i) pnv[i] = 0;

	for(i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		n = f.Nodes();
		for (j=0; j<n; ++j) pnv[ f.node[j] ]++;
	}

	// figure out which face is attached to which node
	int nsize = 0;
	for (i=0; i<nodes; ++i) nsize += pnv[i];

	int* pnf = new int[nsize];
	int** ppnf = new int*[nodes];
	ppnf[0] = pnf;
	for (i=1; i<nodes; ++i)	ppnf[i] = ppnf[i-1] + pnv[i-1];
	for (i=0; i<nodes; ++i) pnv[i] = 0;

	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		n = f.Nodes();
		for (j=0; j<n; ++j)
		{
			int nj = f.node[j];
			ppnf[nj][pnv[nj]] = i;
			pnv[nj]++;
		}
	}

	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];

		// calculate the face normals
		vec3f& r0 = Node(f.node[0]).m_r0;
		vec3f& r1 = Node(f.node[1]).m_r0;
		vec3f& r2 = Node(f.node[2]).m_r0;

		f.m_fn = (r1 - r0)^(r2 - r0);
		f.m_fn.Normalize();

		// clear the neighbours
		f.m_nbr[0] = -1;
		f.m_nbr[1] = -1;
		f.m_nbr[2] = -1;
		f.m_nbr[3] = -1;
	}

	// smoothing threshold
	double eps = cos(m_stol);

	// find neighbours
	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];

		// find all neighbours of this face
		n = f.Edges();
		int jp1;
		int n1, n2, nval;
		for (j=0; j<n; ++j)
		{
			jp1 = (j+1)%n;
			n1 = f.node[j];
			n2 = f.node[jp1];
			nval = pnv[n1];

			for (k=0; k<nval; ++k)
			{
				FEFace& f2 = m_Face[ ppnf[n1][k] ];
				if ((&f2 != &f) && (f2.HasEdge(n1, n2)))
				{
					// we found the neighbour
					// now let's see if it would belong to the smoothing group
					// also make sure that they are of similar type,
					// that is: shells can connect only to shells and solids to solids
					int e1 = (Element(f .m_elem[0]).IsSolid()?1:0);
					int e2 = (Element(f2.m_elem[0]).IsSolid()?1:0);
					if ((f.m_fn*f2.m_fn >= eps) && (e1 == e2))
					{
						// Eureka! We found one!
						f.m_nbr[j] = ppnf[n1][k];
						break;
					}
				}
			}
		}
	}

	// clean up
	delete [] pnv;
	delete [] pnf;
	delete [] ppnf;

	//calculate the node normals
	vector<vec3f> pnorm(nodes);
	for (i=0; i<nodes; ++i) pnorm[i] = vec3f(0,0,0);

	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		f.m_ntag = -1;
	}

	vector<FEFace*> F(faces);
	int NF = 0;

	// unprocessed face list
	int ui = 0;

	stack<FEFace*> stack;
	int nsg = 0;
	FEFace* pf = 0;
	do
	{
		if (stack.empty())
		{
			if (nsg > 0)
			{
				// assign node normals
				for (i=0; i<NF; ++i)
				{
					FEFace& f = *F[i];
					assert(f.m_ntag == nsg);
					int nf = f.Nodes();
					for (int k=0; k<nf; ++k) f.m_nn[k] = pnorm[ f.node[k] ];
				}

				// clear normals
				for (i=0; i<NF; ++i)
				{
					FEFace& f = *F[i];
					int nf = f.Nodes();
					for (int k=0; k<nf; ++k) pnorm[ f.node[k] ] = vec3f(0,0,0);
				}
			}

			// find an unprocessed face
			pf = 0;
			for (i = ui; i<faces; ++i, ++ui) if (Face(i).m_ntag == -1) { pf = &m_Face[i]; break; }

			if (pf) stack.push(pf);
			++nsg;
			NF = 0;
		}
		else
		{
			// pop a face
			pf = stack.top(); stack.pop();

			// mark as processed
			pf->m_ntag = nsg;
			F[NF++] = pf;

			int nf = pf->Nodes();
			if ((nf==3)||(nf==6)) n = 3;
			if ((nf==4)||(nf==8)) n = 4;

			// add face normal to node normal
			for (i=0; i<nf; ++i) pnorm[pf->node[i]] += pf->m_fn;

			// push unprocessed neighbours
			for (i=0; i<n; ++i)
			{
				if (pf->m_nbr[i] >= 0)
				{
					FEFace& f2 = m_Face[pf->m_nbr[i]];
					if (f2.m_ntag == -1)
					{
						f2.m_ntag = -2;
						stack.push(&f2);
					}
				}
			}
		}
	}
	while (pf);

	// normalize face normals
	for (i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		int nf = f.Nodes();
		for (int k=0; k<nf; ++k) f.m_nn[k].Normalize();
	}
}

//-----------------------------------------------------------------------------
// Clear all selection
void FEMesh::ClearSelection()
{
	int i;
	for (i=0; i<Elements(); i++) m_Elem[i].Unselect();
	for (i=0; i<Faces   (); i++) m_Face[i].Unselect();
	for (i=0; i<Nodes   (); i++) m_Node[i].Unselect();
}

//-----------------------------------------------------------------------------
// Count the selected nodes
int FEMesh::CountSelectedNodes()
{
	int i, N = Nodes();
	int count = 0;
	for (i=0; i<N; i++) if (m_Node[i].IsSelected()) count++;
	return count;
}

//-----------------------------------------------------------------------------
// Count the selected elements
int FEMesh::CountSelectedElems()
{
	int i, N = Elements();
	int count = 0;
	for (i=0; i<N; i++) if (m_Elem[i].IsSelected()) count++;
	return count;
}

//-----------------------------------------------------------------------------
// Count the selected faces
int FEMesh::CountSelectedFaces()
{
	int i, N = Faces();
	int count = 0;
	for (i=0; i<N; i++) if (m_Face[i].IsSelected()) count++;
	return count;
}

//-----------------------------------------------------------------------------
// Update the face normals. If bsmooth, the smoothing groups are used
// to create a smoothed surface representation.
void FEMesh::UpdateNormals(bool bsmooth)
{
	int faces = Faces();
	int nodes = Nodes();

	// calculate face normals
	for (int i=0; i<faces; ++i)
	{
		FEFace& face = Face(i);

		vec3f& r1 = Node(face.node[0]).m_rt;
		vec3f& r2 = Node(face.node[1]).m_rt;
		vec3f& r3 = Node(face.node[2]).m_rt;

		face.m_fn = (r2-r1)^(r3-r1);
		face.m_fn.Normalize();
	}

	for (int i=0; i<faces; ++i)
	{
		FEFace& f = m_Face[i];
		f.m_ntag = -1;
	}

	// calculate node normals based on smoothing groups
	if (bsmooth)
	{
		vector<FEFace*> stack(faces);
		int ns = 0;

		int nsg = 0;
		vector<vec3f> nt(nodes);
		vector<int> ntag; ntag.assign(nodes, 0);
		for (int i=0; i<faces; ++i)
		{
			// find the next unprocessed face
			FEFace* pf = &m_Face[i];
			if (pf->m_ntag == -1)
			{
				// push this face on the stack
				stack[ns++] = pf;

				// find all connected faces
				while (ns > 0)
				{
					// pop a face
					pf = stack[--ns];

					// mark as processed
					pf->m_ntag = nsg;

					int fn = pf->Nodes();
					int fe = pf->Edges();

					// add face normal to node normal
					for (int j=0; j<fn; ++j)
					{
						nt[pf->node[j]] += pf->m_fn;
						ntag[pf->node[j]] = 1;
					}

					// push unprocessed neighbors
					FEFace* pf2;
					for (int j=0; j<fe; ++j)
					{	
						if (pf->m_nbr[j] >= 0)
						{
							pf2 = &m_Face[pf->m_nbr[j]];
							if (pf2->m_ntag == -1) stack[ns++] = pf2;
						}
					}
				}

				// normalize normals
				for (int j=0; j<nodes; ++j)
				{
					if (ntag[j] == 1) { nt[j].Normalize(); ntag[j] = 0; }
				}

				// assign node normals
				for (int j=0; j<faces; ++j)
				{
					FEFace& f = m_Face[j];
					if (f.m_ntag == nsg)
					{
						int nf = f.Nodes();
						for (int k=0; k<nf; ++k) f.m_nn[k] = nt[ f.node[k] ];
					}
				}

				// clear normals for next group
				for (int j=0; j<nodes; ++j) nt[j] = vec3f(0,0,0);
				++nsg;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Hide elements with a particular material ID
void FEMesh::HideElements(int nmat)
{
	int i;
	// Hide the elements with the material ID
	for (i=0; i<Elements(); ++i) if (Element(i).m_MatID == nmat) Element(i).Hide();

	// hide faces
	int NF = Faces();
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}

	// hide nodes: nodes will be hidden if all elements they attach to are hidden
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}
}

//-----------------------------------------------------------------------------
// Show elements with a certain material ID
void FEMesh::ShowElements(int nmat)
{
	int i;

	// unhide the elements with mat ID nmat
	int NE = Elements();
	for (i=0; i<NE; ++i) 
	{
		FEElement& e = m_Elem[i];
		if (e.m_MatID == nmat) m_Elem[i].Show();
	}

	// show faces
	int NF = Faces();
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		f.Show();
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}

	// hide nodes: nodes will be hidden if all elements they attach to are hidden
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		m_Node[i].Show();
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}
}

//-----------------------------------------------------------------------------
// Enable elements with a certain mat ID
void FEMesh::EnableElements(int nmat)
{
	// update the elements
	int i, j, n;
	for (i=0; i<Elements(); ++i) if (Element(i).m_MatID == nmat) Element(i).Enable();

	// now we update the nodes
	for (i=0; i<Nodes(); ++i) Node(i).Disable();
	for (i=0; i<Elements(); ++i)
	{
		FEElement& el = Element(i);
		if (el.IsEnabled())
		{
			n = el.Nodes();
			for (j=0; j<n; ++j) Node(el.m_node[j]).Enable();
		}
	}

	// enable the faces
	for (i=0; i<Faces(); ++i) 
	{
		FEFace& f = m_Face[i];
		f.Disable();
		if (m_Elem[f.m_elem[0]].IsEnabled()) f.Enable();
	}
}

//-----------------------------------------------------------------------------
// Disable elements with a certain mat ID
void FEMesh::DisableElements(int nmat)
{
	// update the elements
	int i, j, n;
	for (i=0; i<Elements(); ++i) if (Element(i).m_MatID == nmat) Element(i).Disable();

	// now we update the nodes
	for (i=0; i<Nodes(); ++i) Node(i).Disable();
	for (i=0; i<Elements(); ++i)
	{
		FEElement& el = Element(i);
		if (el.IsEnabled())
		{
			n = el.Nodes();
			for (j=0; j<n; ++j) Node(el.m_node[j]).Enable();
		}
	}

	// enable the faces
	for (i=0; i<Faces(); ++i) 
	{
		FEFace& f = m_Face[i];
		f.Disable();
		if (m_Elem[f.m_elem[0]].IsEnabled()) f.Enable();
	}
}

//-----------------------------------------------------------------------------
// Select nodes from a list
void FEMesh::SelectNodes(std::vector<int> &item, bool bclear)
{
	int i, N = Nodes();

	// clear the current selection
	if (bclear)
	{
		for (i=0; i<N; ++i) m_Node[i].Unselect();
	}

	// select the nodes in the list
	for (i=0; i<(int) item.size(); ++i)
	{
		int n = item[i];
		if ((n >= 0) && (n < N))
		{
			if (m_Node[n].IsVisible()) m_Node[n].Select();
		}
	}
}

//-----------------------------------------------------------------------------
// select the elements from a list
void FEMesh::SelectElements(std::vector<int> &item, bool bclear)
{
	int i, N = Elements();

	// clear the current selection
	if (bclear)
	{
		for (i=0; i<N; ++i) m_Elem[i].Unselect();
	}

	// select the elements in the list
	for (i=0; i<(int) item.size(); ++i)
	{
		int n = item[i];
		if ((n >= 0) && (n < N))
		{
			if (m_Elem[n].IsVisible()) m_Elem[n].Select();
		}
	}
}

//-----------------------------------------------------------------------------
// select the faces from a list
void FEMesh::SelectFaces(std::vector<int> &item, bool bclear)
{
	int i, N = Faces();

	// clear the current selection
	if (bclear)
	{
		for (i=0; i<N; ++i) m_Face[i].Unselect();
	}

	// select the faces in the list
	for (i=0; i<(int) item.size(); ++i)
	{
		int n = item[i];
		if ((n >= 0) && (n < N)) 
		{
			if (m_Face[n].IsVisible()) m_Face[n].Select();
		}
	}
}

//-----------------------------------------------------------------------------
// Select elements that are connected through the surface
void FEMesh::SelectConnectedSurfaceElements(FEElement &el)
{
	int i;
	if (!el.IsVisible()) return;

	// tag all faces
	for (i=0; i<Faces(); ++i) Face(i).m_ntag = 0;

	// find the face that this element belongs to
	for (i=0; i<Faces(); ++i)
	{
		FEFace& f = Face(i);
		if (f.m_elem[0] == el.m_nId)
		{
			// propagate through all neighbors
			stack<FEFace*> S;
			S.push(&f);
			while (!S.empty())
			{
				FEFace* pf = S.top(); S.pop();
				pf->m_ntag = 1;
				FEElement& e2 = m_Elem[pf->m_elem[0]];
				if (e2.IsVisible())
				{
					e2.Select();
					for (int j=0; j<pf->Edges(); ++j)
					{
						FEFace* pf2 = (pf->m_nbr[j] >= 0? &Face(pf->m_nbr[j]) : 0);
						if (pf2 && (pf2->m_ntag == 0)) S.push(pf2);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Select elements that are connected through the volume
void FEMesh::SelectConnectedVolumeElements(FEElement &el)
{
	if (!el.IsVisible()) return;

	// tag all elements
	for (int i=0; i<Elements(); ++i) Element(i).m_ntag = 0;

	// propagate through all neighbors
	stack<FEElement*> S;
	S.push(&el);
	while (!S.empty())
	{
		FEElement* pe = S.top(); S.pop();
		pe->m_ntag = 1;
		pe->Select();
		for (int j=0; j<pe->Faces(); ++j)
		{
			FEElement* pe2 = pe->m_pElem[j];
			if (pe2 && pe2->IsVisible() && (pe2->m_ntag == 0)) S.push(pe2);
		}
	}
}

//-----------------------------------------------------------------------------
// Select faces that are connected
void FEMesh::SelectConnectedFaces(FEFace &f)
{
	int i;

	// clear tags on all faces
	for (i=0; i<Faces(); ++i) Face(i).m_ntag = 0;

	// propagate through all neighbors
	stack<FEFace*> S;
	f.m_ntag = 1;
	S.push(&f);
	while (!S.empty())
	{
		FEFace* pf = S.top(); S.pop();
		FEElement& el = m_Elem[pf->m_elem[0]];
		if (el.IsVisible())
		{
			pf->Select();
			for (int j=0; j<pf->Edges(); ++j)
			{
				FEFace* pf2 = (pf->m_nbr[j] >= 0? &Face(pf->m_nbr[j]) : 0);
				if (pf2 && (pf2->m_ntag == 0)) 
				{
					pf2->m_ntag = 1;
					S.push(pf2);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Select nodes that are connected on a surface
void FEMesh::SelectConnectedSurfaceNodes(int n)
{
	int i;

	// clear tags on all faces
	int NF = Faces();
	for (i=0; i<NF; ++i) Face(i).m_ntag = 0;

	// find a face that has this node connects to
	vector<NodeFaceRef>& nfl = m_NFL.FaceList(n);
	FEFace* pf = 0;
	for (i=0; i<(int) nfl.size(); ++i)
	{
		FEFace& f = m_Face[nfl[i].first];
		if (f.IsVisible())
		{
			pf = &f;
			break;
		}
	}
	if (pf == 0) return;

	// propagate through all neighbors
	stack<FEFace*> S;
	pf->m_ntag = 1;
	S.push(pf);
	while (!S.empty())
	{
		FEFace* pf = S.top(); S.pop();
		if (pf->IsVisible())
		{
			for (int j=0; j<pf->Edges(); ++j)
			{
				FEFace* pf2 = (pf->m_nbr[j] >= 0? &Face(pf->m_nbr[j]) : 0);
				if (pf2 && (pf2->m_ntag == 0)) 
				{
					pf2->m_ntag = 1;
					S.push(pf2);
				}
			}
		}
	}

	// select all the nodes of tagged faces
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (f.m_ntag == 1)
		{
			int nf = f.Nodes();
			for (int j=0; j<nf; ++j) m_Node[f.node[j]].Select();
		}
	}
}

//-----------------------------------------------------------------------------
// Select nodes that are connected on a volume
void FEMesh::SelectConnectedVolumeNodes(int n)
{
	int i;

	// clear tags on all elements
	int NE = Elements();
	for (i=0; i<NE; ++i) Element(i).m_ntag = 0;

	// find a visible element that has this node connects to
	vector<NodeElemRef>& nel = m_NEL.ElemList(n);
	FEElement* pe = 0;
	for (i=0; i<(int) nel.size(); ++i)
	{
		FEElement& e = m_Elem[nel[i].first];
		if (e.IsVisible())
		{
			pe = &e;
			break;
		}
	}
	if (pe == 0) return;

	// propagate through all neighbors
	stack<FEElement*> S;
	pe->m_ntag = 1;
	S.push(pe);
	while (!S.empty())
	{
		FEElement* pe = S.top(); S.pop();
		if (pe->IsVisible())
		{
			for (int j=0; j<pe->Faces(); ++j)
			{
				FEElement* pe2 = pe->m_pElem[j];
				if (pe2 && (pe2->m_ntag == 0)) 
				{
					pe2->m_ntag = 1;
					S.push(pe2);
				}
			}
		}
	}

	// select all the nodes of tagged elements
	for (i=0; i<NE; ++i)
	{
		FEElement& e = m_Elem[i];
		if (e.m_ntag == 1)
		{
			int nf = e.Nodes();
			for (int j=0; j<nf; ++j) m_Node[e.m_node[j]].Select();
		}
	}
}

//-----------------------------------------------------------------------------
// area of triangle
double triangle_area(vec3f& r0, vec3f& r1, vec3f& r2)
{
	return ((r1 - r0)^(r2 - r0)).Length()*0.5f;
}

//-----------------------------------------------------------------------------
// Calculate the area of a FEFace
float FEMesh::FaceArea(FEFace &f)
{
	switch (f.Nodes())
	{
	case 3: 
		{
			vec3f r0 = m_Node[f.node[0]].m_rt;
			vec3f r1 = m_Node[f.node[1]].m_rt;
			vec3f r2 = m_Node[f.node[2]].m_rt;

			return triangle_area(r0, r1, r2);
		}
		break;
	case 6:
		{
			vec3f r0 = m_Node[f.node[0]].m_rt;
			vec3f r1 = m_Node[f.node[1]].m_rt;
			vec3f r2 = m_Node[f.node[2]].m_rt;
			vec3f r3 = m_Node[f.node[3]].m_rt;
			vec3f r4 = m_Node[f.node[4]].m_rt;
			vec3f r5 = m_Node[f.node[5]].m_rt;
			float A = 0.f;
			A += triangle_area(r0, r3, r5);
			A += triangle_area(r3, r1, r4);
			A += triangle_area(r2, r5, r4);
			A += triangle_area(r3, r4, r5);
			return A;
		}
		break;
	case 7:
		{
			vec3f r0 = m_Node[f.node[0]].m_rt;
			vec3f r1 = m_Node[f.node[1]].m_rt;
			vec3f r2 = m_Node[f.node[2]].m_rt;

			return triangle_area(r0, r1, r2);
		}
		break;
	case 4:
		{
			int i, n;

			// gauss-point data
			const float a = 1.f / (float) sqrt(3.0);
			const int NELN = 4;
			const int NINT = 4;
			static float gr[NINT] = { -a,  a,  a, -a };
			static float gs[NINT] = { -a, -a,  a,  a };
			static float gw[NINT] = {  1,  1,  1,  1 };

			static float H[NINT][NELN] = {0};
			static float Gr[NINT][NELN] = {0};
			static float Gs[NINT][NELN] = {0};
			static bool bfirst = true;

			if (bfirst)
			{

				// calculate shape function values at gauss points
				for (n=0; n<NINT; ++n)
				{
					H[n][0] = 0.25f*(1 - gr[n])*(1 - gs[n]);
					H[n][1] = 0.25f*(1 + gr[n])*(1 - gs[n]);
					H[n][2] = 0.25f*(1 + gr[n])*(1 + gs[n]);
					H[n][3] = 0.25f*(1 - gr[n])*(1 + gs[n]);
				}

				// calculate local derivatives of shape functions at gauss points
				for (n=0; n<NINT; ++n)
				{
					Gr[n][0] = -0.25f*(1 - gs[n]);
					Gr[n][1] =  0.25f*(1 - gs[n]);
					Gr[n][2] =  0.25f*(1 + gs[n]);
					Gr[n][3] = -0.25f*(1 + gs[n]);

					Gs[n][0] = -0.25f*(1 - gr[n]);
					Gs[n][1] = -0.25f*(1 + gr[n]);
					Gs[n][2] =  0.25f*(1 + gr[n]);
					Gs[n][3] =  0.25f*(1 - gr[n]);
				}

				bfirst = false;
			}

			vec3f rt[NELN];
			for (i=0; i<NELN; ++i) rt[i] = m_Node[f.node[i]].m_rt;

			float A = 0.f;
			for (n=0; n<NINT; ++n)
			{
				// calculate jacobian
				vec3f dxr, dxs;
				for (i=0; i<NELN; ++i)
				{
					dxr.x += Gr[n][i]*rt[i].x;
					dxr.y += Gr[n][i]*rt[i].y;
					dxr.z += Gr[n][i]*rt[i].z;

					dxs.x += Gs[n][i]*rt[i].x;
					dxs.y += Gs[n][i]*rt[i].y;
					dxs.z += Gs[n][i]*rt[i].z;
				}

				float detJ = (dxr ^ dxs).Length();

				A += gw[n]*detJ;
			}

			return A;
		}
		break;
	}

	return 0.0;
}


//-----------------------------------------------------------------------------
// Calculate the volume of an element
float FEMesh::ElementVolume(int iel)
{
	FEElement& el = Element(iel);
	switch (el.m_ntype)
	{
	case FE_HEX8  : return HexVolume(el); break;
	case FE_HEX20 : return HexVolume(el); break;
	case FE_HEX27 : return HexVolume(el); break;
	case FE_TET4  : return TetVolume(el); break;
	case FE_PENTA6: return PentaVolume(el); break;
	}

	return 0.f;
}

//-----------------------------------------------------------------------------
// Calculate the volume of a hex element
float FEMesh::HexVolume(FEElement& el)
{
	assert((el.m_ntype == FE_HEX8) || (el.m_ntype == FE_HEX20) || (el.m_ntype == FE_HEX27));

	// gauss-point data
	const float a = 1.f / (float) sqrt(3.0);
	const int NELN = 8;
	const int NINT = 8;
	static float gr[NINT] = { -a,  a,  a, -a, -a,  a, a, -a };
	static float gs[NINT] = { -a, -a,  a,  a, -a, -a, a,  a };
	static float gt[NINT] = { -a, -a, -a, -a,  a,  a, a,  a };
	static float gw[NINT] = {  1,  1,  1,  1,  1,  1,  1, 1 };

	static float H[NINT][NELN] = {0};
	static float Gr[NINT][NELN] = {0};
	static float Gs[NINT][NELN] = {0};
	static float Gt[NINT][NELN] = {0};
	static bool bfirst = true;

	if (bfirst)
	{
		int n;

		// calculate shape function values at gauss points
		for (n=0; n<NINT; ++n)
		{
			H[n][0] = 0.125f*(1 - gr[n])*(1 - gs[n])*(1 - gt[n]);
			H[n][1] = 0.125f*(1 + gr[n])*(1 - gs[n])*(1 - gt[n]);
			H[n][2] = 0.125f*(1 + gr[n])*(1 + gs[n])*(1 - gt[n]);
			H[n][3] = 0.125f*(1 - gr[n])*(1 + gs[n])*(1 - gt[n]);
			H[n][4] = 0.125f*(1 - gr[n])*(1 - gs[n])*(1 + gt[n]);
			H[n][5] = 0.125f*(1 + gr[n])*(1 - gs[n])*(1 + gt[n]);
			H[n][6] = 0.125f*(1 + gr[n])*(1 + gs[n])*(1 + gt[n]);
			H[n][7] = 0.125f*(1 - gr[n])*(1 + gs[n])*(1 + gt[n]);
		}

		// calculate local derivatives of shape functions at gauss points
		for (n=0; n<NINT; ++n)
		{
			Gr[n][0] = -0.125f*(1 - gs[n])*(1 - gt[n]);
			Gr[n][1] =  0.125f*(1 - gs[n])*(1 - gt[n]);
			Gr[n][2] =  0.125f*(1 + gs[n])*(1 - gt[n]);
			Gr[n][3] = -0.125f*(1 + gs[n])*(1 - gt[n]);
			Gr[n][4] = -0.125f*(1 - gs[n])*(1 + gt[n]);
			Gr[n][5] =  0.125f*(1 - gs[n])*(1 + gt[n]);
			Gr[n][6] =  0.125f*(1 + gs[n])*(1 + gt[n]);
			Gr[n][7] = -0.125f*(1 + gs[n])*(1 + gt[n]);

			Gs[n][0] = -0.125f*(1 - gr[n])*(1 - gt[n]);
			Gs[n][1] = -0.125f*(1 + gr[n])*(1 - gt[n]);
			Gs[n][2] =  0.125f*(1 + gr[n])*(1 - gt[n]);
			Gs[n][3] =  0.125f*(1 - gr[n])*(1 - gt[n]);
			Gs[n][4] = -0.125f*(1 - gr[n])*(1 + gt[n]);
			Gs[n][5] = -0.125f*(1 + gr[n])*(1 + gt[n]);
			Gs[n][6] =  0.125f*(1 + gr[n])*(1 + gt[n]);
			Gs[n][7] =  0.125f*(1 - gr[n])*(1 + gt[n]);

			Gt[n][0] = -0.125f*(1 - gr[n])*(1 - gs[n]);
			Gt[n][1] = -0.125f*(1 + gr[n])*(1 - gs[n]);
			Gt[n][2] = -0.125f*(1 + gr[n])*(1 + gs[n]);
			Gt[n][3] = -0.125f*(1 - gr[n])*(1 + gs[n]);
			Gt[n][4] =  0.125f*(1 - gr[n])*(1 - gs[n]);
			Gt[n][5] =  0.125f*(1 + gr[n])*(1 - gs[n]);
			Gt[n][6] =  0.125f*(1 + gr[n])*(1 + gs[n]);
			Gt[n][7] =  0.125f*(1 - gr[n])*(1 + gs[n]);
		}

		bfirst = false;
	}

	float *Grn, *Gsn, *Gtn;
	float vol = 0, detJ;
	float J[3][3];
	int i, n;

	vec3f rt[NELN];
	for (i=0; i<NELN; ++i) rt[i] = m_Node[el.m_node[i]].m_rt;

	for (n=0; n<NINT; ++n)
	{
		Grn = Gr[n];
		Gsn = Gs[n];
		Gtn = Gt[n];

		J[0][0] = J[0][1] = J[0][2] = 0.0;
		J[1][0] = J[1][1] = J[1][2] = 0.0;
		J[2][0] = J[2][1] = J[2][2] = 0.0;
		for (i=0; i<NELN; ++i)
		{
			const float& Gri = Grn[i];
			const float& Gsi = Gsn[i];
			const float& Gti = Gtn[i];

			const float& x = rt[i].x;
			const float& y = rt[i].y;
			const float& z = rt[i].z;

			J[0][0] += Gri*x; J[0][1] += Gsi*x; J[0][2] += Gti*x;
			J[1][0] += Gri*y; J[1][1] += Gsi*y; J[1][2] += Gti*y;
			J[2][0] += Gri*z; J[2][1] += Gsi*z; J[2][2] += Gti*z;
		}

		// calculate the determinant
		detJ = J[0][0]*(J[1][1]*J[2][2] - J[1][2]*J[2][1]) 
			+ J[0][1]*(J[1][2]*J[2][0] - J[2][2]*J[1][0]) 
			+ J[0][2]*(J[1][0]*J[2][1] - J[1][1]*J[2][0]);

		vol += detJ*gw[n];
	}

	return vol;
}

//-----------------------------------------------------------------------------
// Calculate the volume of a pentahedral element
float FEMesh::PentaVolume(FEElement& el)
{
	assert(el.m_ntype == FE_PENTA6);

	// gauss-point data
	//gauss intergration points
	const float a = 1.f/6.f;
	const float b = 2.f/3.f;
	const float c = 1.f / (float) sqrt(3.0);
	const float w = 1.f / 6.f;

	const int NELN = 6;
	const int NINT = 6;

	static float gr[NINT] = { a, b, a, a, b, a };
	static float gs[NINT] = { a, a, b, a, a, b };
	static float gt[NINT] = { -c, -c, -c, c, c, c };
	static float gw[NINT] = { w, w, w, w, w, w };

	static float H[NINT][NELN] = {0};
	static float Gr[NINT][NELN] = {0};
	static float Gs[NINT][NELN] = {0};
	static float Gt[NINT][NELN] = {0};
	static bool bfirst = true;

	if (bfirst)
	{
		int n;

		// calculate shape function values at gauss points
		for (n=0; n<NINT; ++n)
		{
			H[n][0] = 0.5f*(1.f - gt[n])*(1.f - gr[n] - gs[n]);
			H[n][1] = 0.5f*(1.f - gt[n])*gr[n];
			H[n][2] = 0.5f*(1.f - gt[n])*gs[n];
			H[n][3] = 0.5f*(1.f + gt[n])*(1.f - gr[n] - gs[n]);
			H[n][4] = 0.5f*(1.f + gt[n])*gr[n];
			H[n][5] = 0.5f*(1.f + gt[n])*gs[n];
		}

		// calculate local derivatives of shape functions at gauss points
		for (n=0; n<NINT; ++n)
		{
			Gr[n][0] = -0.5f*(1.f - gt[n]);
			Gr[n][1] =  0.5f*(1.f - gt[n]);
			Gr[n][2] =  0.0f;
			Gr[n][3] = -0.5f*(1.f + gt[n]);
			Gr[n][4] =  0.5f*(1.f + gt[n]);
			Gr[n][5] =  0.0f;

			Gs[n][0] = -0.5f*(1.f - gt[n]);
			Gs[n][1] =  0.0f;
			Gs[n][2] =  0.5f*(1.f - gt[n]);
			Gs[n][3] = -0.5f*(1.f + gt[n]);
			Gs[n][4] =  0.0f;
			Gs[n][5] =  0.5f*(1.f + gt[n]);

			Gt[n][0] = -0.5f*(1.f - gr[n] - gs[n]);
			Gt[n][1] = -0.5f*gr[n];
			Gt[n][2] = -0.5f*gs[n];
			Gt[n][3] =  0.5f*(1.f - gr[n] - gs[n]);
			Gt[n][4] =  0.5f*gr[n];
			Gt[n][5] =  0.5f*gs[n];
		}

		bfirst = false;
	}

	float *Grn, *Gsn, *Gtn;
	float vol = 0, detJ;
	float J[3][3];
	int i, n;

	vec3f rt[NELN];
	for (i=0; i<NELN; ++i) rt[i] = m_Node[el.m_node[i]].m_rt;

	for (n=0; n<NINT; ++n)
	{
		Grn = Gr[n];
		Gsn = Gs[n];
		Gtn = Gt[n];

		J[0][0] = J[0][1] = J[0][2] = 0.0;
		J[1][0] = J[1][1] = J[1][2] = 0.0;
		J[2][0] = J[2][1] = J[2][2] = 0.0;
		for (i=0; i<NELN; ++i)
		{
			const float& Gri = Grn[i];
			const float& Gsi = Gsn[i];
			const float& Gti = Gtn[i];

			const float& x = rt[i].x;
			const float& y = rt[i].y;
			const float& z = rt[i].z;

			J[0][0] += Gri*x; J[0][1] += Gsi*x; J[0][2] += Gti*x;
			J[1][0] += Gri*y; J[1][1] += Gsi*y; J[1][2] += Gti*y;
			J[2][0] += Gri*z; J[2][1] += Gsi*z; J[2][2] += Gti*z;
		}

		// calculate the determinant
		detJ = J[0][0]*(J[1][1]*J[2][2] - J[1][2]*J[2][1]) 
			+ J[0][1]*(J[1][2]*J[2][0] - J[2][2]*J[1][0]) 
			+ J[0][2]*(J[1][0]*J[2][1] - J[1][1]*J[2][0]);

		vol += detJ*gw[n];
	}

	return vol;
}

//-----------------------------------------------------------------------------
// Calculate the volume of a tetrahedral element
float FEMesh::TetVolume(FEElement& el)
{
	assert(el.m_ntype == FE_TET4);

	// gauss-point data
	const float a = 0.58541020f;
	const float b = 0.13819660f;
	const float w = 1.f / 24.f;

	const int NELN = 4;
	const int NINT = 4;

	static float gr[NINT] = { b, a, b, b };
	static float gs[NINT] = { b, b, a, b };
	static float gt[NINT] = { b, b, b, a };
	static float gw[NINT] = { w, w, w, w };

	static float H[NINT][NELN] = {0};
	static float Gr[NINT][NELN] = {0};
	static float Gs[NINT][NELN] = {0};
	static float Gt[NINT][NELN] = {0};
	static bool bfirst = true;

	if (bfirst)
	{
		int n;

		// calculate shape function values at gauss points
		for (n=0; n<NINT; ++n)
		{
			H[n][0] = 1.f - gr[n] - gs[n] - gt[n];
			H[n][1] = gr[n];
			H[n][2] = gs[n];
			H[n][3] = gt[n];
		}

		// calculate local derivatives of shape functions at gauss points
		for (n=0; n<NINT; ++n)
		{
			Gr[n][0] = -1.f;
			Gr[n][1] =  1.f;
			Gr[n][2] =  0.f;
			Gr[n][3] =  0.f;

			Gs[n][0] = -1.f;
			Gs[n][1] =  0.f;
			Gs[n][2] =  1.f;
			Gs[n][3] =  0.f;

			Gt[n][0] = -1.f;
			Gt[n][1] =  0.f;
			Gt[n][2] =  0.f;
			Gt[n][3] =  1.f;
		}

		bfirst = false;
	}

	float *Grn, *Gsn, *Gtn;
	float vol = 0, detJ;
	float J[3][3];
	int i, n;

	vec3f rt[NELN];
	for (i=0; i<NELN; ++i) rt[i] = m_Node[el.m_node[i]].m_rt;

	for (n=0; n<NINT; ++n)
	{
		Grn = Gr[n];
		Gsn = Gs[n];
		Gtn = Gt[n];

		J[0][0] = J[0][1] = J[0][2] = 0.0;
		J[1][0] = J[1][1] = J[1][2] = 0.0;
		J[2][0] = J[2][1] = J[2][2] = 0.0;
		for (i=0; i<NELN; ++i)
		{
			const float& Gri = Grn[i];
			const float& Gsi = Gsn[i];
			const float& Gti = Gtn[i];

			const float& x = rt[i].x;
			const float& y = rt[i].y;
			const float& z = rt[i].z;

			J[0][0] += Gri*x; J[0][1] += Gsi*x; J[0][2] += Gti*x;
			J[1][0] += Gri*y; J[1][1] += Gsi*y; J[1][2] += Gti*y;
			J[2][0] += Gri*z; J[2][1] += Gsi*z; J[2][2] += Gti*z;
		}

		// calculate the determinant
		detJ = J[0][0]*(J[1][1]*J[2][2] - J[1][2]*J[2][1]) 
			+ J[0][1]*(J[1][2]*J[2][0] - J[2][2]*J[1][0]) 
			+ J[0][2]*(J[1][0]*J[2][1] - J[1][1]*J[2][0]);

		vol += detJ*gw[n];
	}

	return vol;
}

//-----------------------------------------------------------------------------
// Hide selected elements
void FEMesh::HideSelectedElements()
{
	int i;

	// hide selected elements
	int NE = Elements();
	for (i=0; i<NE; i++)
	{
		FEElement& e = m_Elem[i];
		if (e.IsSelected()) e.Hide();
	}

	// hide nodes: nodes will be hidden if all elements they attach to are hidden
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}

	// hide faces
	int NF = Faces();
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}
}

//-----------------------------------------------------------------------------
// Hide selected elements
void FEMesh::HideUnselectedElements()
{
	int i;

	// hide unselected elements
	int NE = Elements();
	for (i=0; i<NE; i++)
	{
		FEElement& e = m_Elem[i];
		if (!e.IsSelected()) e.Hide();
	}

	// hide nodes: nodes will be hidden if all elements they attach to are hidden
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}

	// hide faces
	int NF = Faces();
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}
}

//-----------------------------------------------------------------------------
// Hide selected faces
void FEMesh::HideSelectedFaces()
{
	int i;
	// hide the faces and the elements that they are attached to
	int NF = Faces();
	for (i=0; i<NF; ++i) 
	{
		FEFace& f = m_Face[i];
		if (f.IsSelected())
		{
			f.Hide();
			m_Elem[f.m_elem[0]].Hide();
		}
	}

	// hide nodes: nodes will be hidden if all elements they attach to are hidden
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}

	// hide faces that were hidden by hiding the elements
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}
}

//-----------------------------------------------------------------------------
// hide selected nodes
void FEMesh::HideSelectedNodes()
{
	int i;

	// hide nodes and all elements they attach to
	int NN = Nodes();
	for (i=0; i<NN; ++i)
	{
		FENode& n = m_Node[i];
		if (n.IsSelected())
		{
			n.Hide();
			vector<NodeElemRef>& nel = m_NEL.ElemList(i);
			int ne = nel.size();
			for (int j=0; j<ne; ++j) m_Elem[nel[j].first].Hide();
		}
	}

	// hide nodes that were hidden by hiding elements
	for (i=0; i<NN; ++i)
	{
		vector<NodeElemRef>& nel = m_NEL.ElemList(i);
		int ne = nel.size();
		bool bhide = true;
		for (int j=0; j<ne; ++j)
		{
			if (m_Elem[nel[j].first].IsVisible()) { bhide = false; break; }
		}
		if (bhide) m_Node[i].Hide();
	}

	// hide faces that were hidden by hiding the elements
	int NF = Faces();
	for (i=0; i<NF; ++i)
	{
		FEFace& f = m_Face[i];
		if (m_Elem[f.m_elem[0]].IsVisible() == false) f.Hide();
	}
}

//-----------------------------------------------------------------------------
void FEMesh::FaceNodePosition(FEFace& f, vec3f* r)
{
	switch (f.m_ntype)
	{
	case FACE_QUAD9:
		r[8] = m_Node[f.node[8]].m_rt;
	case FACE_QUAD8:
		r[7] = m_Node[f.node[7]].m_rt;
	case FACE_TRI7:
		r[6] = m_Node[f.node[6]].m_rt;
	case FACE_TRI6:
		r[5] = m_Node[f.node[5]].m_rt;
		r[4] = m_Node[f.node[4]].m_rt;
	case FACE_QUAD4:
		r[3] = m_Node[f.node[3]].m_rt;
	case FACE_TRI3:
		r[2] = m_Node[f.node[2]].m_rt;
		r[1] = m_Node[f.node[1]].m_rt;
		r[0] = m_Node[f.node[0]].m_rt;
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void FEMesh::FaceNodeNormals(FEFace& f, vec3f* n)
{
	switch (f.m_ntype)
	{
	case FACE_QUAD9:
		n[8] = f.m_nn[8];
	case FACE_QUAD8:
		n[7] = f.m_nn[7];
	case FACE_TRI7:
		n[6] = f.m_nn[6];
	case FACE_TRI6:
		n[5] = f.m_nn[5];
		n[4] = f.m_nn[4];
	case FACE_QUAD4:
		n[3] = f.m_nn[3];
	case FACE_TRI3:
		n[2] = f.m_nn[2];
		n[1] = f.m_nn[1];
		n[0] = f.m_nn[0];
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void FEMesh::FaceNodeTexCoords(FEFace& f, float* t, bool bnode)
{
	if (bnode)
	{
		for (int i=0; i<f.Nodes(); ++i) t[i] = m_Node[f.node[i]].m_tex;
	}
	else
	{
		for (int i=0; i<f.Nodes(); ++i) t[i] = f.m_tex[i];
	}
}

//-----------------------------------------------------------------------------
bool IsInsideElement(FEElement& el, double r[3], const double tol)
{
	switch (el.m_ntype)
	{
	case FE_TET4:
	case FE_TET10:
	case FE_TET15:
		return (r[0] >= -tol)&&(r[1] >= -tol)&&(r[2] >= -tol)&&(r[0]+r[1]+r[2] <= 1.0+tol);
	case FE_HEX8:
	case FE_HEX20:
	case FE_HEX27:
		return ((r[0]>=-1.0-tol)&&(r[0]<= 1.0+tol)&&
			    (r[1]>=-1.0-tol)&&(r[1]<= 1.0+tol)&&
				(r[2]>=-1.0-tol)&&(r[2]<= 1.0+tol));
	}
	return false;
}

//-----------------------------------------------------------------------------
bool ProjectInsideElement(FEMesh& m, FEElement& el, const vec3f& p, double r[3])
{
	const double tol = 0.0001;
	const int nmax = 10;
	r[0] = r[1] = r[2] = 0.f;
	double dr[3], R[3];
	int ne = el.Nodes();
	vec3f x[FEElement::MAX_NODES];
	for (int i=0; i<ne; ++i) x[i] = m.Node(el.m_node[i]).m_rt;
	mat3d K, Ki;
	double u2, N[FEElement::MAX_NODES], G[3][FEElement::MAX_NODES];
	int n = 0;
	do
	{
		el.shape(N, r[0], r[1], r[2]);
		el.shape_deriv(G[0], G[1], G[2], r[0], r[1], r[2]);

		R[0] = p.x;
		R[1] = p.y;
		R[2] = p.z;
		for (int i=0; i<ne; ++i)
		{
			R[0] -= N[i]*x[i].x;
			R[1] -= N[i]*x[i].y;
			R[2] -= N[i]*x[i].z;
		}

		K.zero();
		for (int i=0; i<ne; ++i)
		{
			K[0][0] -= G[0][i]*x[i].x; K[0][1] -= G[1][i]*x[i].x; K[0][2] -= G[2][i]*x[i].x;
			K[1][0] -= G[0][i]*x[i].y; K[1][1] -= G[1][i]*x[i].y; K[1][2] -= G[2][i]*x[i].y;
			K[2][0] -= G[0][i]*x[i].z; K[2][1] -= G[1][i]*x[i].z; K[2][2] -= G[2][i]*x[i].z;
		}

		Ki = K;
		Ki.Invert();

		dr[0] = Ki[0][0]*R[0] + Ki[0][1]*R[1] + Ki[0][2]*R[2];
		dr[1] = Ki[1][0]*R[0] + Ki[1][1]*R[1] + Ki[1][2]*R[2];
		dr[2] = Ki[2][0]*R[0] + Ki[2][1]*R[1] + Ki[2][2]*R[2];

		r[0] -= dr[0];
		r[1] -= dr[1];
		r[2] -= dr[2];

		u2 = dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2];
		++n;
	}
	while ((u2 > tol*tol)&&(n < nmax));

	return IsInsideElement(el, r, 0.001);
}

//-----------------------------------------------------------------------------
bool FindElementRef(FEMesh& m, const vec3f& p, int& nelem, double r[3])
{
	vec3f y[FEElement::MAX_NODES];
	int NE = m.Elements();
	for (int i=0; i<NE; ++i)
	{
		FEElement& e = m.Element(i);
		int ne = e.Nodes();
		nelem = i;

		// do a quick bounding box test
		vec3f r0 = m.Node(e.m_node[0]).m_rt;
		vec3f r1 = r0;
		for (int j=1; j<ne; ++j)
		{
			vec3f& rj = m.Node(e.m_node[j]).m_rt;
			if (rj.x < r0.x) r0.x = rj.x;
			if (rj.y < r0.y) r0.y = rj.y;
			if (rj.z < r0.z) r0.z = rj.z;
			if (rj.x > r1.x) r1.x = rj.x;
			if (rj.y > r1.y) r1.y = rj.y;
			if (rj.z > r1.z) r1.z = rj.z;
		}

		float dx = fabs(r0.x - r1.x);
		float dy = fabs(r0.y - r1.y);
		float dz = fabs(r0.z - r1.z);

		float R = dx;
		if (dy > R) R = dy;
		if (dz > R) R = dz;
		float eps = R*0.001f;

		r0.x -= eps;
		r0.y -= eps;
		r0.z -= eps;

		r1.x += eps;
		r1.y += eps;
		r1.z += eps;

		if ((p.x  >= r0.x)&&(p.x <= r1.x)&&
			(p.y  >= r0.y)&&(p.y <= r1.y)&&
			(p.z  >= r0.z)&&(p.z <= r1.z))
		{
			if (ProjectInsideElement(m, e, p, r)) return true;	
		}
	}

	return false;
}
