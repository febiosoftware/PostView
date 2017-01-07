// FEElement.cpp: implementation of the FEElement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEElement.h"

//-----------------------------------------------------------------------------
// Face lookup tables
int FT_HEX[6][4] = {
	{0, 1, 5, 4},
	{1, 2, 6, 5},
	{3, 0, 4, 7},
	{1, 0, 3, 2},
	{4, 5, 6, 7},
	{2, 3, 7, 6}};

int FT_PENTA[5][4] = {
	{0, 1, 4, 3},
	{1, 2, 5, 4},
	{2, 0, 3, 5},
	{2, 1, 0, 0},
	{3, 4, 5, 5}};

int FT_TET[4][4] = {
	{2, 1, 0, 0},
	{0, 1, 3, 3},
	{1, 2, 3, 3},
	{2, 0, 3, 3}};

int FT_HEX20[6][8] = {
	{0, 1, 5, 4,  8, 17, 12, 16},
	{1, 2, 6, 5,  9, 18, 13, 17},
	{2, 3, 7, 6, 10, 19, 14, 18},
	{3, 0, 4, 7, 11, 16, 15, 19},
	{3, 2, 1, 0, 10,  9,  8, 11},
	{4, 5, 6, 7, 12, 13, 14, 15}};

int FT_HEX27[6][9] = {
	{0, 1, 5, 4,  8, 17, 12, 16, 20},
	{1, 2, 6, 5,  9, 18, 13, 17, 21},
	{2, 3, 7, 6, 10, 19, 14, 18, 22},
	{3, 0, 4, 7, 11, 16, 15, 19, 23},
	{3, 2, 1, 0, 10,  9,  8, 11, 24},
	{4, 5, 6, 7, 12, 13, 14, 15, 25}};

int FT_TET10[4][6] = {
	{0, 1, 3, 4, 8, 7},
	{1, 2, 3, 5, 9, 8},
	{2, 0, 3, 6, 7, 9},
	{2, 1, 0, 5, 4, 6}};

int FT_TET15[4][7] = {
	{0, 1, 3, 4, 8, 7, 11},
	{1, 2, 3, 5, 9, 8, 12},
	{2, 0, 3, 6, 7, 9, 13},
	{2, 1, 0, 5, 4, 6, 10}};

int FT_TET20[4][10] = {
	{ 0, 1, 3, 4, 5, 12, 13, 10, 11, 16 },
	{ 1, 2, 3, 6, 7, 14, 15, 12, 13, 17 },
	{ 2, 0, 3, 9, 8, 10, 11, 14, 15, 18 },
	{ 2, 1, 0, 7, 6,  5,  4,  9,  8, 19 }};
	
const int ET_QUAD[4][2] = {
	{ 0, 1},
	{ 1, 2},
	{ 2, 3},
	{ 3, 0}};

const int ET_QUAD8[4][3] = {
    { 0, 1, 4},
    { 1, 2, 5},
    { 2, 3, 6},
    { 3, 0, 7}};

const int ET_TRI[3][2] = {
	{ 0, 1},
	{ 1, 2},
	{ 2, 3}};

const int ET_TRI6[3][3] = {
    { 0, 1, 3},
    { 1, 2, 4},
    { 2, 0, 5}};

//=============================================================================
// FEEdge
//-----------------------------------------------------------------------------
FEEdge::FEEdge()
{
	node[0] = node[1] = node[2] = -1;
}

//-----------------------------------------------------------------------------
//! Evaluate the shape function values at the iso-parametric point r = [0,1]
void FEEdge::shape(double* H, double r)
{
	switch (m_type)
	{
	case EDGE_LINE2:
		H[0] = 1.0 - r;
		H[1] = r;
		break;
	case EDGE_LINE3:
		H[0] = (1-r)*(2*(1-r)-1);
		H[1] = r*(2*r - 1);
		H[2] = 4*(1-r)*r;
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
double FEEdge::eval(double* d, double r)
{
	double H[FEEdge::MAX_NODES];
	shape(H, r);
	double a = 0.0;
	for (int i=0; i<Nodes(); ++i) a += H[i]*d[i];
	return a;
}

//-----------------------------------------------------------------------------
vec3f FEEdge::eval(vec3f* d, double r)
{
	double H[FEEdge::MAX_NODES];
	shape(H, r);
	vec3f a(0,0,0);
	for (int i=0; i<Nodes(); ++i) a += d[i]*((float)H[i]);
	return a;
}

//=============================================================================
// FEFace
//-----------------------------------------------------------------------------
FEFace::FEFace()
{
	m_nsg = 0;
	m_ntype = -1;
	for (int i=0; i<MAX_NODES; ++i) node[i] = -1;
}

//-----------------------------------------------------------------------------
//! return the edge
FEEdge FEFace::Edge(int i)
{
	FEEdge e;
	assert(i<Edges());
	switch (m_ntype)
	{
	case FACE_TRI3: 
		{
			const int L[3][2] = {{0,1},{1,2},{2,0}};
			e.node[0] = node[L[i][0]]; e.node[1] = node[L[i][1]];
			e.m_type = EDGE_LINE2;
		}
		break;
	case FACE_QUAD4: 
		{
			const int L[4][2] = {{0,1},{1,2},{2,3},{3,0}};
			e.node[0] = node[L[i][0]]; e.node[1] = node[L[i][1]];
			e.m_type = EDGE_LINE2;
		}
		break;
	case FACE_TRI6:
	case FACE_TRI7:
		{
			const int L[3][3] = {{0,1,3},{1,2,4},{2,0,5}};
			e.node[0] = node[L[i][0]]; e.node[1] = node[L[i][1]]; e.node[2] = node[L[i][2]];
			e.m_type = EDGE_LINE3;
		}
		break;
	case FACE_QUAD8:
	case FACE_QUAD9:
		{
			const int L[4][3] = {{0,1,4},{1,2,5},{2,3,6},{3,0,7}};
			e.node[0] = node[L[i][0]]; e.node[1] = node[L[i][1]]; e.node[2] = node[L[i][2]];
			e.m_type = EDGE_LINE3;
		}
		break;
	case FACE_TRI10:
		{
			const int L[3][4] = {{0,1,3,4},{1,2,5,6},{2,0,8,7}};
			e.m_type = EDGE_LINE4;
			e.node[0] = node[L[i][0]];
			e.node[1] = node[L[i][1]];
			e.node[2] = node[L[i][2]];
			e.node[3] = node[L[i][3]];
		}
		break;
	default:
		assert(false);
	}
	return e;
}

//-----------------------------------------------------------------------------
//! Evaluate the shape function values at the iso-parametric point (r,s)
void FEFace::shape(double* H, double r, double s)
{
	switch (m_ntype)
	{
	case FACE_TRI3:
		{
			H[0] = 1.0 - r - s;
			H[1] = r;
			H[2] = s;
		}
		break;
	case FACE_QUAD4:
		{
			H[0] = 0.25*(1.0 - r)*(1.0 - s);
			H[1] = 0.25*(1.0 + r)*(1.0 - s);
			H[2] = 0.25*(1.0 + r)*(1.0 + s);
			H[3] = 0.25*(1.0 - r)*(1.0 + s);
		}
		break;
	case FACE_TRI6:
		{
			double r1 = 1.0 - r - s;
			double r2 = r;
			double r3 = s;

			H[0] = r1*(2.0*r1 - 1.0);
			H[1] = r2*(2.0*r2 - 1.0);
			H[2] = r3*(2.0*r3 - 1.0);
			H[3] = 4.0*r1*r2;
			H[4] = 4.0*r2*r3;
			H[5] = 4.0*r3*r1;
		}
		break;
	case FACE_TRI7:
		{
			double r1 = 1.0 - r - s;
			double r2 = r;
			double r3 = s;

			H[6] = 27.0*r1*r2*r3;
			H[0] = r1*(2.0*r1 - 1.0) + H[6]/9.0;
			H[1] = r2*(2.0*r2 - 1.0) + H[6]/9.0;
			H[2] = r3*(2.0*r3 - 1.0) + H[6]/9.0;
			H[3] = 4.0*r1*r2 - 4.0*H[6]/9.0;
			H[4] = 4.0*r2*r3 - 4.0*H[6]/9.0;
			H[5] = 4.0*r3*r1 - 4.0*H[6]/9.0;
		}
		break;
	case FACE_QUAD8:
		{
			H[4] = 0.5*(1 - r*r)*(1 - s);
			H[5] = 0.5*(1 - s*s)*(1 + r);
			H[6] = 0.5*(1 - r*r)*(1 + s);
			H[7] = 0.5*(1 - s*s)*(1 - r);

			H[0] = 0.25*(1 - r)*(1 - s) - 0.5*(H[4] + H[7]);
			H[1] = 0.25*(1 + r)*(1 - s) - 0.5*(H[4] + H[5]);
			H[2] = 0.25*(1 + r)*(1 + s) - 0.5*(H[5] + H[6]);
			H[3] = 0.25*(1 - r)*(1 + s) - 0.5*(H[6] + H[7]);
		}
		break;
	case FACE_QUAD9:
		{
			double R[3] = {0.5*r*(r - 1.0), 0.5*r*(r+1.0), 1.0 - r*r};
			double S[3] = {0.5*s*(s - 1.0), 0.5*s*(s+1.0), 1.0 - s*s};

			H[0] = R[0]*S[0];
			H[1] = R[1]*S[0];
			H[2] = R[1]*S[1];
			H[3] = R[0]*S[1];
			H[4] = R[2]*S[0];
			H[5] = R[1]*S[2];
			H[6] = R[2]*S[1];
			H[7] = R[0]*S[2];
			H[8] = R[2]*S[2];
		}
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
double FEFace::eval(double* d, double r, double s)
{
	double H[FEFace::MAX_NODES];
	shape(H, r, s);
	double a = 0.0;
	for (int i=0; i<Nodes(); ++i) a += H[i]*d[i];
	return a;
}

//-----------------------------------------------------------------------------
vec3f FEFace::eval(vec3f* d, double r, double s)
{
	double H[FEFace::MAX_NODES];
	shape(H, r, s);
	vec3f a(0,0,0);
	for (int i=0; i<Nodes(); ++i) a += d[i]*((float)H[i]);
	return a;
}

//=============================================================================
FEGenericElement::FEGenericElement()
{
	m_nodes = 0;
	m_faces = 0;
	m_edges = 0;
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = -1; 
}

FEGenericElement::FEGenericElement(const FEGenericElement& e) : FEElement(e)
{
	m_nodes = e.m_nodes;
	m_faces = e.m_faces;
	m_edges = e.m_edges;
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = e.m_node[i]; 
}

void FEGenericElement::operator = (const FEGenericElement& e)
{
	m_nodes = e.m_nodes;
	m_faces = e.m_faces;
	m_edges = e.m_edges;
	FEElement::operator = (e);
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = e.m_node[i]; 
}

void FEGenericElement::SetType(FEElemType type) 
{ 
	m_ntype = type; 
	switch (type)
	{
	case FE_LINE2 : m_nodes = FEElementTraits<FE_LINE2 >::Nodes; m_faces = FEElementTraits<FE_LINE2 >::Faces; m_edges = FEElementTraits<FE_LINE2 >::Edges; break;
	case FE_LINE3 : m_nodes = FEElementTraits<FE_LINE3 >::Nodes; m_faces = FEElementTraits<FE_LINE3 >::Faces; m_edges = FEElementTraits<FE_LINE3 >::Edges; break;
	case FE_TRI3  : m_nodes = FEElementTraits<FE_TRI3  >::Nodes; m_faces = FEElementTraits<FE_TRI3  >::Faces; m_edges = FEElementTraits<FE_TRI3  >::Edges; break;
	case FE_TRI6  : m_nodes = FEElementTraits<FE_TRI6  >::Nodes; m_faces = FEElementTraits<FE_TRI6  >::Faces; m_edges = FEElementTraits<FE_TRI6  >::Edges; break;
	case FE_QUAD4 : m_nodes = FEElementTraits<FE_QUAD4 >::Nodes; m_faces = FEElementTraits<FE_QUAD4 >::Faces; m_edges = FEElementTraits<FE_QUAD4 >::Edges; break;
	case FE_QUAD8 : m_nodes = FEElementTraits<FE_QUAD8 >::Nodes; m_faces = FEElementTraits<FE_QUAD8 >::Faces; m_edges = FEElementTraits<FE_QUAD8 >::Edges; break;
	case FE_QUAD9 : m_nodes = FEElementTraits<FE_QUAD9 >::Nodes; m_faces = FEElementTraits<FE_QUAD9 >::Faces; m_edges = FEElementTraits<FE_QUAD9 >::Edges; break;
	case FE_TET4  : m_nodes = FEElementTraits<FE_TET4  >::Nodes; m_faces = FEElementTraits<FE_TET4  >::Faces; m_edges = FEElementTraits<FE_TET4  >::Edges; break;
	case FE_TET10 : m_nodes = FEElementTraits<FE_TET10 >::Nodes; m_faces = FEElementTraits<FE_TET10 >::Faces; m_edges = FEElementTraits<FE_TET10 >::Edges; break;
	case FE_TET15 : m_nodes = FEElementTraits<FE_TET15 >::Nodes; m_faces = FEElementTraits<FE_TET15 >::Faces; m_edges = FEElementTraits<FE_TET15 >::Edges; break;
	case FE_TET20 : m_nodes = FEElementTraits<FE_TET20 >::Nodes; m_faces = FEElementTraits<FE_TET20 >::Faces; m_edges = FEElementTraits<FE_TET20 >::Edges; break;
	case FE_PENTA6: m_nodes = FEElementTraits<FE_PENTA6>::Nodes; m_faces = FEElementTraits<FE_PENTA6>::Faces; m_edges = FEElementTraits<FE_PENTA6>::Edges; break;
	case FE_HEX8  : m_nodes = FEElementTraits<FE_HEX8  >::Nodes; m_faces = FEElementTraits<FE_HEX8  >::Faces; m_edges = FEElementTraits<FE_HEX8  >::Edges; break;
	case FE_HEX20 : m_nodes = FEElementTraits<FE_HEX20 >::Nodes; m_faces = FEElementTraits<FE_HEX20 >::Faces; m_edges = FEElementTraits<FE_HEX20 >::Edges; break;
	case FE_HEX27 : m_nodes = FEElementTraits<FE_HEX27 >::Nodes; m_faces = FEElementTraits<FE_HEX27 >::Faces; m_edges = FEElementTraits<FE_HEX27 >::Edges; break;
	default:
		assert(false);
		m_nodes = 0;
		m_faces = 0;
	}
}

//=============================================================================
FELinearElement::FELinearElement()
{
	m_nodes = 0;
	m_faces = 0;
	m_edges = 0;
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = -1; 
}

FELinearElement::FELinearElement(const FELinearElement& e) : FEElement(e)
{
	m_nodes = e.m_nodes;
	m_faces = e.m_faces;
	m_edges = e.m_edges;
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = e.m_node[i]; 
}

void FELinearElement::operator = (const FELinearElement& e)
{
	m_nodes = e.m_nodes;
	m_faces = e.m_faces;
	m_edges = e.m_edges;
	FEElement::operator = (e);
	m_node = _node;
	for (int i=0; i<MAX_NODES; ++i) m_node[i] = e.m_node[i]; 
}

void FELinearElement::SetType(FEElemType type) 
{ 
	m_ntype = type; 
	switch (type)
	{
	case FE_LINE2 : m_nodes = FEElementTraits<FE_LINE2 >::Nodes; m_faces = FEElementTraits<FE_LINE2 >::Faces; m_edges = FEElementTraits<FE_LINE2 >::Edges; break;
	case FE_LINE3 : m_nodes = FEElementTraits<FE_LINE3 >::Nodes; m_faces = FEElementTraits<FE_LINE3 >::Faces; m_edges = FEElementTraits<FE_LINE3 >::Edges; break;
	case FE_TRI3  : m_nodes = FEElementTraits<FE_TRI3  >::Nodes; m_faces = FEElementTraits<FE_TRI3  >::Faces; m_edges = FEElementTraits<FE_TRI3  >::Edges; break;
	case FE_QUAD4 : m_nodes = FEElementTraits<FE_QUAD4 >::Nodes; m_faces = FEElementTraits<FE_QUAD4 >::Faces; m_edges = FEElementTraits<FE_QUAD4 >::Edges; break;
	case FE_TET4  : m_nodes = FEElementTraits<FE_TET4  >::Nodes; m_faces = FEElementTraits<FE_TET4  >::Faces; m_edges = FEElementTraits<FE_TET4  >::Edges; break;
	case FE_PENTA6: m_nodes = FEElementTraits<FE_PENTA6>::Nodes; m_faces = FEElementTraits<FE_PENTA6>::Faces; m_edges = FEElementTraits<FE_PENTA6>::Edges; break;
	case FE_HEX8  : m_nodes = FEElementTraits<FE_HEX8  >::Nodes; m_faces = FEElementTraits<FE_HEX8  >::Faces; m_edges = FEElementTraits<FE_HEX8  >::Edges; break;
	default:
		assert(false);
		m_nodes = 0;
		m_faces = 0;
	}
}

//=============================================================================
// FEElement
//-----------------------------------------------------------------------------
FEElement::FEElement()
{
	m_pElem[0] = 0; 
	m_pElem[1] = 0; 
	m_pElem[2] = 0; 
	m_pElem[3] = 0; 
	m_pElem[4] = 0; 
	m_pElem[5] = 0; 
	m_MatID = 0; 
}

//-----------------------------------------------------------------------------
bool FEElement::HasNode(int node) const
{ 
	bool ret = false;
	const int n = Nodes();
	for (int i=0; i<n; i++) ret |= !(m_node[i] ^ node);
	return ret;
}

//-----------------------------------------------------------------------------
// Return a face of the element
FEFace FEElement::GetFace(int i) const
{
	FEFace f;
	GetFace(i, f);
	return f;
}

//-----------------------------------------------------------------------------
// Return a face of the element
void FEElement::GetFace(int i, FEFace& f) const
{
	switch (m_ntype)
	{
	case FE_HEX8:
		f.m_ntype = FACE_QUAD4;
		f.node[0] = m_node[FT_HEX[i][0]];
		f.node[1] = m_node[FT_HEX[i][1]];
		f.node[2] = m_node[FT_HEX[i][2]];
		f.node[3] = m_node[FT_HEX[i][3]];
		break;
	case FE_PENTA6:
		{
			const int ft[5] = {FACE_QUAD4, FACE_QUAD4, FACE_QUAD4, FACE_TRI3, FACE_TRI3};
			f.m_ntype = ft[i];
			f.node[0] = m_node[FT_PENTA[i][0]];
			f.node[1] = m_node[FT_PENTA[i][1]];
			f.node[2] = m_node[FT_PENTA[i][2]];
			f.node[3] = m_node[FT_PENTA[i][3]];
		}
		break;

	case FE_TET4:
		f.m_ntype = FACE_TRI3;
		f.node[0] = m_node[FT_TET[i][0]];
		f.node[1] = m_node[FT_TET[i][1]];
		f.node[2] = m_node[FT_TET[i][2]];
		f.node[3] = m_node[FT_TET[i][3]];
		break;

	case FE_HEX20:
		f.m_ntype = FACE_QUAD8;
		f.node[0] = m_node[FT_HEX20[i][0]];
		f.node[1] = m_node[FT_HEX20[i][1]];
		f.node[2] = m_node[FT_HEX20[i][2]];
		f.node[3] = m_node[FT_HEX20[i][3]];
		f.node[4] = m_node[FT_HEX20[i][4]];
		f.node[5] = m_node[FT_HEX20[i][5]];
		f.node[6] = m_node[FT_HEX20[i][6]];
		f.node[7] = m_node[FT_HEX20[i][7]];
		break;

	case FE_HEX27:
		f.m_ntype = FACE_QUAD9;
		f.node[0] = m_node[FT_HEX27[i][0]];
		f.node[1] = m_node[FT_HEX27[i][1]];
		f.node[2] = m_node[FT_HEX27[i][2]];
		f.node[3] = m_node[FT_HEX27[i][3]];
		f.node[4] = m_node[FT_HEX27[i][4]];
		f.node[5] = m_node[FT_HEX27[i][5]];
		f.node[6] = m_node[FT_HEX27[i][6]];
		f.node[7] = m_node[FT_HEX27[i][7]];
		f.node[8] = m_node[FT_HEX27[i][8]];
		break;

	case FE_TET10:
		f.m_ntype = FACE_TRI6;
		f.node[0] = m_node[FT_TET10[i][0]];
		f.node[1] = m_node[FT_TET10[i][1]];
		f.node[2] = m_node[FT_TET10[i][2]];
		f.node[3] = m_node[FT_TET10[i][3]];
		f.node[4] = m_node[FT_TET10[i][4]];
		f.node[5] = m_node[FT_TET10[i][5]];
		break;

	case FE_TET15:
		f.m_ntype = FACE_TRI7;
		f.node[0] = m_node[FT_TET15[i][0]];
		f.node[1] = m_node[FT_TET15[i][1]];
		f.node[2] = m_node[FT_TET15[i][2]];
		f.node[3] = m_node[FT_TET15[i][3]];
		f.node[4] = m_node[FT_TET15[i][4]];
		f.node[5] = m_node[FT_TET15[i][5]];
		f.node[6] = m_node[FT_TET15[i][6]];
		break;

	case FE_TET20:
		f.m_ntype = FACE_TRI10;
		f.node[0] = m_node[FT_TET20[i][0]];
		f.node[1] = m_node[FT_TET20[i][1]];
		f.node[2] = m_node[FT_TET20[i][2]];
		f.node[3] = m_node[FT_TET20[i][3]];
		f.node[4] = m_node[FT_TET20[i][4]];
		f.node[5] = m_node[FT_TET20[i][5]];
		f.node[6] = m_node[FT_TET20[i][6]];
		f.node[7] = m_node[FT_TET20[i][7]];
		f.node[8] = m_node[FT_TET20[i][8]];
		f.node[9] = m_node[FT_TET20[i][9]];
		break;
	};
}

//-----------------------------------------------------------------------------
// Return an edge of the element
FEEdge FEElement::GetEdge(int i) const
{
	FEEdge e;
	switch(m_ntype)
	{
	case FE_QUAD4:
	case FE_TRI3:
		e.node[0] = m_node[ET_QUAD[i][0]];
		e.node[1] = m_node[ET_QUAD[i][1]];
		break;
    case FE_TRI6:
        e.node[0] = m_node[ET_TRI6[i][0]];
        e.node[1] = m_node[ET_TRI6[i][1]];
        e.node[2] = m_node[ET_TRI6[i][2]];
        break;
    case FE_QUAD8:
    case FE_QUAD9:
        e.node[0] = m_node[ET_QUAD8[i][0]];
        e.node[1] = m_node[ET_QUAD8[i][1]];
        e.node[2] = m_node[ET_QUAD8[i][2]];
        break;
	};
	return e;
}

//-----------------------------------------------------------------------------
// Check comparison between two elements
bool FEElement::operator != (FEElement& e)
{
	if (m_ntype != e.m_ntype) return true;

	switch (m_ntype)
	{
	case FE_HEX8:
	case FE_HEX20:
	case FE_HEX27:
		if ((m_node[0] != e.m_node[0]) ||
			(m_node[1] != e.m_node[1]) ||
			(m_node[2] != e.m_node[2]) ||
			(m_node[3] != e.m_node[3]) ||
			(m_node[4] != e.m_node[4]) ||
			(m_node[5] != e.m_node[5]) ||
			(m_node[6] != e.m_node[6]) ||
			(m_node[7] != e.m_node[7])) return true;
		break;
	case FE_PENTA6:
		if ((m_node[0] != e.m_node[0]) ||
			(m_node[1] != e.m_node[1]) ||
			(m_node[2] != e.m_node[2]) ||
			(m_node[3] != e.m_node[3]) ||
			(m_node[4] != e.m_node[4]) ||
			(m_node[5] != e.m_node[5])) return true;
		break;
	case FE_TET4:
	case FE_QUAD4:
    case FE_QUAD8:
    case FE_QUAD9:
	case FE_TET10:
	case FE_TET15:
	case FE_TET20:
		if ((m_node[0] != e.m_node[0]) ||
			(m_node[1] != e.m_node[1]) ||
			(m_node[2] != e.m_node[2]) ||
			(m_node[3] != e.m_node[3])) return true;
		break;
	case FE_TRI3:
    case FE_TRI6:
		if ((m_node[0] != e.m_node[0]) ||
			(m_node[1] != e.m_node[1]) ||
			(m_node[2] != e.m_node[2])) return true;
		break;
	}

	return false;
}

//-----------------------------------------------------------------------------
//! Calculate the shape function values at the point (r,s,t)
void FEElement::shape(double *H, double r, double s, double t)
{
	switch (m_ntype)
	{
	case FE_TET4:
		{
			H[0] = 1 - r - s - t;
			H[1] = r;
			H[2] = s;
			H[3] = t;
		}
		break;
	case FE_PENTA6:
		{
			H[0] = 0.5*(1 - t)*(1 - r - s);
			H[1] = 0.5*(1 - t)*r;
			H[2] = 0.5*(1 - t)*s;
			H[3] = 0.5*(1 + t)*(1 - r - s);
			H[4] = 0.5*(1 + t)*r;
			H[5] = 0.5*(1 + t)*s;
		}
		break;
	case FE_HEX8:
		{
			H[0] = 0.125*(1 - r)*(1 - s)*(1 - t);
			H[1] = 0.125*(1 + r)*(1 - s)*(1 - t);
			H[2] = 0.125*(1 + r)*(1 + s)*(1 - t);
			H[3] = 0.125*(1 - r)*(1 + s)*(1 - t);
			H[4] = 0.125*(1 - r)*(1 - s)*(1 + t);
			H[5] = 0.125*(1 + r)*(1 - s)*(1 + t);
			H[6] = 0.125*(1 + r)*(1 + s)*(1 + t);
			H[7] = 0.125*(1 - r)*(1 + s)*(1 + t);
		}
		break;
	case FE_TET10:
		{
			double r1 = 1.0 - r - s - t;
			double r2 = r;
			double r3 = s;
			double r4 = t;

			H[0] = r1*(2.0*r1 - 1.0);
			H[1] = r2*(2.0*r2 - 1.0);
			H[2] = r3*(2.0*r3 - 1.0);
			H[3] = r4*(2.0*r4 - 1.0);
			H[4] = 4.0*r1*r2;
			H[5] = 4.0*r2*r3;
			H[6] = 4.0*r3*r1;
			H[7] = 4.0*r1*r4;
			H[8] = 4.0*r2*r4;
			H[9] = 4.0*r3*r4;
		}
		break;
	case FE_TET15:
		{
			double r1 = 1.0 - r - s - t;
			double r2 = r;
			double r3 = s;
			double r4 = t;

			H[14] = 256*r1*r2*r3*r4;

			H[10] = 27.0*r1*r2*r3;
			H[11] = 27.0*r1*r2*r4;
			H[12] = 27.0*r2*r3*r4;
			H[13] = 27.0*r3*r1*r4;

			H[0] = r1*(2.0*r1 - 1.0) + (H[10] + H[11] + H[13])/9.0 - H[14]/64.0;
			H[1] = r2*(2.0*r2 - 1.0) + (H[10] + H[11] + H[12])/9.0 - H[14]/64.0;
			H[2] = r3*(2.0*r3 - 1.0) + (H[10] + H[12] + H[13])/9.0 - H[14]/64.0;
			H[3] = r4*(2.0*r4 - 1.0) + (H[11] + H[12] + H[13])/9.0 - H[14]/64.0;

			H[4] = 4.0*r1*r2 - 4.0*(H[10] + H[11])/9.0 + H[14]/8.0;
			H[5] = 4.0*r2*r3 - 4.0*(H[10] + H[12])/9.0 + H[14]/8.0;
			H[6] = 4.0*r3*r1 - 4.0*(H[10] + H[13])/9.0 + H[14]/8.0;
			H[7] = 4.0*r1*r4 - 4.0*(H[11] + H[13])/9.0 + H[14]/8.0;
			H[8] = 4.0*r2*r4 - 4.0*(H[11] + H[12])/9.0 + H[14]/8.0;
			H[9] = 4.0*r3*r4 - 4.0*(H[12] + H[13])/9.0 + H[14]/8.0;

			H[10] -= 27.0*H[14]/64.0;
			H[11] -= 27.0*H[14]/64.0;
			H[12] -= 27.0*H[14]/64.0;
			H[13] -= 27.0*H[14]/64.0;
		}
		break;
	case FE_TET20:
		{
			double L1 = 1.0 - r - s - t;
			double L2 = r;
			double L3 = s;
			double L4 = t;

			H[0] = 0.5*(3*L1 - 1)*(3*L1 - 2)*L1;
			H[1] = 0.5*(3*L2 - 1)*(3*L2 - 2)*L2;
			H[2] = 0.5*(3*L3 - 1)*(3*L3 - 2)*L3;
			H[3] = 0.5*(3*L4 - 1)*(3*L4 - 2)*L4;
			H[4] = 9.0/2.0*(3*L1 - 1)*L1*L2;
			H[5] = 9.0/2.0*(3*L2 - 1)*L1*L2;
			H[6] = 9.0/2.0*(3*L2 - 1)*L2*L3;
			H[7] = 9.0/2.0*(3*L3 - 1)*L2*L3;
			H[8] = 9.0/2.0*(3*L1 - 1)*L1*L3;
			H[9] = 9.0/2.0*(3*L3 - 1)*L1*L3;
			H[10] = 9.0/2.0*(3*L1 - 1)*L1*L4;
			H[11] = 9.0/2.0*(3*L4 - 1)*L1*L4;
			H[12] = 9.0/2.0*(3*L2 - 1)*L2*L4;
			H[13] = 9.0/2.0*(3*L4 - 1)*L2*L4;
			H[14] = 9.0/2.0*(3*L3 - 1)*L3*L4;
			H[15] = 9.0/2.0*(3*L4 - 1)*L3*L4;
			H[16] = 27.0*L1*L2*L4;
			H[17] = 27.0*L2*L3*L4;
			H[18] = 27.0*L1*L3*L4;
			H[19] = 27.0*L1*L2*L3;
		}
		break;
	case FE_HEX20:
		{
			H[ 8] = 0.25*(1 - r*r)*(1 - s)*(1 - t);
			H[ 9] = 0.25*(1 - s*s)*(1 + r)*(1 - t);
			H[10] = 0.25*(1 - r*r)*(1 + s)*(1 - t);
			H[11] = 0.25*(1 - s*s)*(1 - r)*(1 - t);
			H[12] = 0.25*(1 - r*r)*(1 - s)*(1 + t);
			H[13] = 0.25*(1 - s*s)*(1 + r)*(1 + t);
			H[14] = 0.25*(1 - r*r)*(1 + s)*(1 + t);
			H[15] = 0.25*(1 - s*s)*(1 - r)*(1 + t);
			H[16] = 0.25*(1 - t*t)*(1 - r)*(1 - s);
			H[17] = 0.25*(1 - t*t)*(1 + r)*(1 - s);
			H[18] = 0.25*(1 - t*t)*(1 + r)*(1 + s);
			H[19] = 0.25*(1 - t*t)*(1 - r)*(1 + s);

			H[0] = 0.125*(1 - r)*(1 - s)*(1 - t) - 0.5*(H[ 8] + H[11] + H[16]);
			H[1] = 0.125*(1 + r)*(1 - s)*(1 - t) - 0.5*(H[ 8] + H[ 9] + H[17]);
			H[2] = 0.125*(1 + r)*(1 + s)*(1 - t) - 0.5*(H[ 9] + H[10] + H[18]);
			H[3] = 0.125*(1 - r)*(1 + s)*(1 - t) - 0.5*(H[10] + H[11] + H[19]);
			H[4] = 0.125*(1 - r)*(1 - s)*(1 + t) - 0.5*(H[12] + H[15] + H[16]);
			H[5] = 0.125*(1 + r)*(1 - s)*(1 + t) - 0.5*(H[12] + H[13] + H[17]);
			H[6] = 0.125*(1 + r)*(1 + s)*(1 + t) - 0.5*(H[13] + H[14] + H[18]);
			H[7] = 0.125*(1 - r)*(1 + s)*(1 + t) - 0.5*(H[14] + H[15] + H[19]);
		}
		break;
	case FE_HEX27:
		{
			double R[3] = {0.5*r*(r-1.0), 0.5*r*(r+1.0), 1.0 - r*r};
			double S[3] = {0.5*s*(s-1.0), 0.5*s*(s+1.0), 1.0 - s*s};
			double T[3] = {0.5*t*(t-1.0), 0.5*t*(t+1.0), 1.0 - t*t};

			H[ 0] = R[0]*S[0]*T[0];
			H[ 1] = R[1]*S[0]*T[0];
			H[ 2] = R[1]*S[1]*T[0];
			H[ 3] = R[0]*S[1]*T[0];
			H[ 4] = R[0]*S[0]*T[1];
			H[ 5] = R[1]*S[0]*T[1];
			H[ 6] = R[1]*S[1]*T[1];
			H[ 7] = R[0]*S[1]*T[1];
			H[ 8] = R[2]*S[0]*T[0];
			H[ 9] = R[1]*S[2]*T[0];
			H[10] = R[2]*S[1]*T[0];
			H[11] = R[0]*S[2]*T[0];
			H[12] = R[2]*S[0]*T[1];
			H[13] = R[1]*S[2]*T[1];
			H[14] = R[2]*S[1]*T[1];
			H[15] = R[0]*S[2]*T[1];
			H[16] = R[0]*S[0]*T[2];
			H[17] = R[1]*S[0]*T[2];
			H[18] = R[1]*S[1]*T[2];
			H[19] = R[0]*S[1]*T[2];
			H[20] = R[2]*S[0]*T[2];
			H[21] = R[1]*S[2]*T[2];
			H[22] = R[2]*S[1]*T[2];
			H[23] = R[0]*S[2]*T[2];
			H[24] = R[2]*S[2]*T[0];
			H[25] = R[2]*S[2]*T[1];
			H[26] = R[2]*S[2]*T[2];		
		}
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
double FEElement::eval(double* d, double r, double s, double t)
{
	double H[FEGenericElement::MAX_NODES];
	shape(H, r, s, t);
	double a = 0.0;
	for (int i=0; i<Nodes(); ++i) a += H[i]*d[i];
	return a;
}

//-----------------------------------------------------------------------------
vec3f FEElement::eval(vec3f* d, double r, double s, double t)
{
	double H[FEGenericElement::MAX_NODES];
	shape(H, r, s, t);
	vec3f a(0,0,0);
	for (int i=0; i<Nodes(); ++i) a += d[i]*((float)H[i]);
	return a;
}

//-----------------------------------------------------------------------------
void FEElement::shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t)
{
	switch (m_ntype)
	{
	case FE_TET4:
		{
			Hr[0] = -1; Hs[0] = -1; Ht[0] = -1;
			Hr[1] =  1;	Hs[1] =  0; Ht[1] =  0;
			Hr[2] =  0;	Hs[2] =  1; Ht[2] =  0;
			Hr[3] =  0;	Hs[3] =  0; Ht[3] =  1;
		}
		break;
	case FE_PENTA6:
		{
			Hr[0] = -0.5*(1 - t); Hs[0] = -0.5*(1 - t); Ht[0] = -0.5*(1 - r - s);
			Hr[1] =  0.5*(1 - t); Hs[1] =  0.0        ;	Ht[1] = -0.5*r;
			Hr[2] =  0.0        ; Hs[2] =  0.5*(1 - t);	Ht[2] = -0.5*s;
			Hr[3] = -0.5*(1 + t); Hs[3] = -0.5*(1 + t); Ht[3] =  0.5*(1 - r - s);
			Hr[4] =  0.5*(1 + t); Hs[4] =  0.0        ; Ht[4] =  0.5*r;
			Hr[5] =  0.0        ; Hs[5] =  0.5*(1 + t);	Ht[5] =  0.5*s;
		}
		break;
	case FE_HEX8:
		{ 
			Hr[0] = -0.125*(1 - s)*(1 - t); Hs[0] = -0.125*(1 - r)*(1 - t); Ht[0] = -0.125*(1 - r)*(1 - s);
			Hr[1] =  0.125*(1 - s)*(1 - t);	Hs[1] = -0.125*(1 + r)*(1 - t);	Ht[1] = -0.125*(1 + r)*(1 - s);
			Hr[2] =  0.125*(1 + s)*(1 - t);	Hs[2] =  0.125*(1 + r)*(1 - t);	Ht[2] = -0.125*(1 + r)*(1 + s);
			Hr[3] = -0.125*(1 + s)*(1 - t);	Hs[3] =  0.125*(1 - r)*(1 - t);	Ht[3] = -0.125*(1 - r)*(1 + s);
			Hr[4] = -0.125*(1 - s)*(1 + t);	Hs[4] = -0.125*(1 - r)*(1 + t);	Ht[4] =  0.125*(1 - r)*(1 - s);
			Hr[5] =  0.125*(1 - s)*(1 + t);	Hs[5] = -0.125*(1 + r)*(1 + t);	Ht[5] =  0.125*(1 + r)*(1 - s);
			Hr[6] =  0.125*(1 + s)*(1 + t);	Hs[6] =  0.125*(1 + r)*(1 + t);	Ht[6] =  0.125*(1 + r)*(1 + s);
			Hr[7] = -0.125*(1 + s)*(1 + t);	Hs[7] =  0.125*(1 - r)*(1 + t);	Ht[7] =  0.125*(1 - r)*(1 + s);
		}
		break;
	case FE_TET10:
		{
			Hr[0] = -3.0 + 4.0*r + 4.0*(s + t);
			Hr[1] =  4.0*r - 1.0;
			Hr[2] =  0.0;
			Hr[3] =  0.0;
			Hr[4] =  4.0 - 8.0*r - 4.0*(s + t);
			Hr[5] =  4.0*s;
			Hr[6] = -4.0*s;
			Hr[7] = -4.0*t;
			Hr[8] =  4.0*t;
			Hr[9] =  0.0;

			Hs[0] = -3.0 + 4.0*s + 4.0*(r + t);
			Hs[1] =  0.0;
			Hs[2] =  4.0*s - 1.0;
			Hs[3] =  0.0;
			Hs[4] = -4.0*r;
			Hs[5] =  4.0*r;
			Hs[6] =  4.0 - 8.0*s - 4.0*(r + t);
			Hs[7] = -4.0*t;
			Hs[8] =  0.0;
			Hs[9] =  4.0*t;

			Ht[0] = -3.0 + 4.0*t + 4.0*(r + s);
			Ht[1] =  0.0;
			Ht[2] =  0.0;
			Ht[3] =  4.0*t - 1.0;
			Ht[4] = -4.0*r;
			Ht[5] =  0.0;
			Ht[6] = -4.0*s;
			Ht[7] =  4.0 - 8.0*t - 4.0*(r + s);
			Ht[8] =  4.0*r;
			Ht[9] =  4.0*s;
		}
		break;
	case FE_TET15:
		{
			Hr[14] = 256.0*s*t*(1.0 - 2.0*r - s - t);
			Hs[14] = 256.0*r*t*(1.0 - r - 2.0*s - t);
			Ht[14] = 256.0*r*s*(1.0 - r - s - 2.0*t);

			Hr[10] =  27.0*s*(1.0 - 2.0*r - s - t);
			Hr[11] =  27.0*t*(1.0 - 2.0*r - s - t);
			Hr[12] =  27.0*s*t;
			Hr[13] = -27.0*s*t;

			Hs[10] =  27.0*r*(1.0 - r - 2.0*s - t);
			Hs[11] = -27.0*r*t;
			Hs[12] =  27.0*r*t;
			Hs[13] =  27.0*t*(1.0 - r - 2.0*s - t);

			Ht[10] = -27.0*r*s;
			Ht[11] =  27.0*r*(1.0 - r - s - 2.0*t);
			Ht[12] =  27.0*r*s;
			Ht[13] =  27.0*s*(1.0 - r - s - 2.0*t);

			Hr[0] = -3.0 + 4.0*r + 4.0*(s + t) + (Hr[10] + Hr[11] + Hr[13])/9.0 - Hr[14]/64.0;
			Hr[1] =  4.0*r - 1.0			   + (Hr[10] + Hr[11] + Hr[12])/9.0 - Hr[14]/64.0;
			Hr[2] =  0.0					   + (Hr[10] + Hr[12] + Hr[13])/9.0 - Hr[14]/64.0;
			Hr[3] =  0.0					   + (Hr[11] + Hr[12] + Hr[13])/9.0 - Hr[14]/64.0;
			Hr[4] =  4.0 - 8.0*r - 4.0*(s + t) - 4.0*(Hr[10] + Hr[11])/9.0 + Hr[14]/8.0;
			Hr[5] =  4.0*s					   - 4.0*(Hr[10] + Hr[12])/9.0 + Hr[14]/8.0;
			Hr[6] = -4.0*s					   - 4.0*(Hr[10] + Hr[13])/9.0 + Hr[14]/8.0;
			Hr[7] = -4.0*t					   - 4.0*(Hr[11] + Hr[13])/9.0 + Hr[14]/8.0;
			Hr[8] =  4.0*t					   - 4.0*(Hr[11] + Hr[12])/9.0 + Hr[14]/8.0;
			Hr[9] =  0.0					   - 4.0*(Hr[12] + Hr[13])/9.0 + Hr[14]/8.0;

			Hs[0] = -3.0 + 4.0*s + 4.0*(r + t) + (Hs[10] + Hs[11] + Hs[13])/9.0 - Hs[14]/64.0;
			Hs[1] =  0.0					   + (Hs[10] + Hs[11] + Hs[12])/9.0 - Hs[14]/64.0;
			Hs[2] =  4.0*s - 1.0			   + (Hs[10] + Hs[12] + Hs[13])/9.0 - Hs[14]/64.0;
			Hs[3] =  0.0					   + (Hs[11] + Hs[12] + Hs[13])/9.0 - Hs[14]/64.0;
			Hs[4] = -4.0*r					   - 4.0*(Hs[10] + Hs[11])/9.0 + Hs[14]/8.0;
			Hs[5] =  4.0*r					   - 4.0*(Hs[10] + Hs[12])/9.0 + Hs[14]/8.0;
			Hs[6] =  4.0 - 8.0*s - 4.0*(r + t) - 4.0*(Hs[10] + Hs[13])/9.0 + Hs[14]/8.0;
			Hs[7] = -4.0*t					   - 4.0*(Hs[11] + Hs[13])/9.0 + Hs[14]/8.0;
			Hs[8] =  0.0					   - 4.0*(Hs[11] + Hs[12])/9.0 + Hs[14]/8.0;
			Hs[9] =  4.0*t					   - 4.0*(Hs[12] + Hs[13])/9.0 + Hs[14]/8.0;

			Ht[0] = -3.0 + 4.0*t + 4.0*(r + s) + (Ht[10] + Ht[11] + Ht[13])/9.0 - Ht[14]/64.0;
			Ht[1] =  0.0					   + (Ht[10] + Ht[11] + Ht[12])/9.0 - Ht[14]/64.0;
			Ht[2] =  0.0					   + (Ht[10] + Ht[12] + Ht[13])/9.0 - Ht[14]/64.0;
			Ht[3] =  4.0*t - 1.0			   + (Ht[11] + Ht[12] + Ht[13])/9.0 - Ht[14]/64.0;
			Ht[4] = -4.0*r					   - 4.0*(Ht[10] + Ht[11])/9.0 + Ht[14]/8.0;
			Ht[5] =  0.0					   - 4.0*(Ht[10] + Ht[12])/9.0 + Ht[14]/8.0;
			Ht[6] = -4.0*s					   - 4.0*(Ht[10] + Ht[13])/9.0 + Ht[14]/8.0;
			Ht[7] =  4.0 - 8.0*t - 4.0*(r + s) - 4.0*(Ht[11] + Ht[13])/9.0 + Ht[14]/8.0;
			Ht[8] =  4.0*r					   - 4.0*(Ht[11] + Ht[12])/9.0 + Ht[14]/8.0;
			Ht[9] =  4.0*s					   - 4.0*(Ht[12] + Ht[13])/9.0 + Ht[14]/8.0;

			Hr[10] -= 27.0*Hr[14]/64.0;
			Hr[11] -= 27.0*Hr[14]/64.0;
			Hr[12] -= 27.0*Hr[14]/64.0;
			Hr[13] -= 27.0*Hr[14]/64.0;

			Hs[10] -= 27.0*Hs[14]/64.0;
			Hs[11] -= 27.0*Hs[14]/64.0;
			Hs[12] -= 27.0*Hs[14]/64.0;
			Hs[13] -= 27.0*Hs[14]/64.0;

			Ht[10] -= 27.0*Ht[14]/64.0;
			Ht[11] -= 27.0*Ht[14]/64.0;
			Ht[12] -= 27.0*Ht[14]/64.0;
			Ht[13] -= 27.0*Ht[14]/64.0;
		}
		break;
	case FE_TET20:
		{
			// TODO: Implement this
			assert(false);
		}
		break;
	case FE_HEX20:
		{
			Hr[ 8] = -0.5*r*(1 - s)*(1 - t);
			Hr[ 9] =  0.25*(1 - s*s)*(1 - t);
			Hr[10] = -0.5*r*(1 + s)*(1 - t);
			Hr[11] = -0.25*(1 - s*s)*(1 - t);
			Hr[12] = -0.5*r*(1 - s)*(1 + t);
			Hr[13] =  0.25*(1 - s*s)*(1 + t);
			Hr[14] = -0.5*r*(1 + s)*(1 + t);
			Hr[15] = -0.25*(1 - s*s)*(1 + t);
			Hr[16] = -0.25*(1 - t*t)*(1 - s);
			Hr[17] =  0.25*(1 - t*t)*(1 - s);
			Hr[18] =  0.25*(1 - t*t)*(1 + s);
			Hr[19] = -0.25*(1 - t*t)*(1 + s);

			Hr[0] = -0.125*(1 - s)*(1 - t) - 0.5*(Hr[ 8] + Hr[11] + Hr[16]);
			Hr[1] =  0.125*(1 - s)*(1 - t) - 0.5*(Hr[ 8] + Hr[ 9] + Hr[17]);
			Hr[2] =  0.125*(1 + s)*(1 - t) - 0.5*(Hr[ 9] + Hr[10] + Hr[18]);
			Hr[3] = -0.125*(1 + s)*(1 - t) - 0.5*(Hr[10] + Hr[11] + Hr[19]);
			Hr[4] = -0.125*(1 - s)*(1 + t) - 0.5*(Hr[12] + Hr[15] + Hr[16]);
			Hr[5] =  0.125*(1 - s)*(1 + t) - 0.5*(Hr[12] + Hr[13] + Hr[17]);
			Hr[6] =  0.125*(1 + s)*(1 + t) - 0.5*(Hr[13] + Hr[14] + Hr[18]);
			Hr[7] = -0.125*(1 + s)*(1 + t) - 0.5*(Hr[14] + Hr[15] + Hr[19]);
				
			Hs[ 8] = -0.25*(1 - r*r)*(1 - t);
			Hs[ 9] = -0.5*s*(1 + r)*(1 - t);
			Hs[10] = 0.25*(1 - r*r)*(1 - t);
			Hs[11] = -0.5*s*(1 - r)*(1 - t);
			Hs[12] = -0.25*(1 - r*r)*(1 + t);
			Hs[13] = -0.5*s*(1 + r)*(1 + t);
			Hs[14] = 0.25*(1 - r*r)*(1 + t);
			Hs[15] = -0.5*s*(1 - r)*(1 + t);
			Hs[16] = -0.25*(1 - t*t)*(1 - r);
			Hs[17] = -0.25*(1 - t*t)*(1 + r);
			Hs[18] =  0.25*(1 - t*t)*(1 + r);
			Hs[19] =  0.25*(1 - t*t)*(1 - r);

			Hs[0] = -0.125*(1 - r)*(1 - t) - 0.5*(Hs[ 8] + Hs[11] + Hs[16]);
			Hs[1] = -0.125*(1 + r)*(1 - t) - 0.5*(Hs[ 8] + Hs[ 9] + Hs[17]);
			Hs[2] =  0.125*(1 + r)*(1 - t) - 0.5*(Hs[ 9] + Hs[10] + Hs[18]);
			Hs[3] =  0.125*(1 - r)*(1 - t) - 0.5*(Hs[10] + Hs[11] + Hs[19]);
			Hs[4] = -0.125*(1 - r)*(1 + t) - 0.5*(Hs[12] + Hs[15] + Hs[16]);
			Hs[5] = -0.125*(1 + r)*(1 + t) - 0.5*(Hs[12] + Hs[13] + Hs[17]);
			Hs[6] =  0.125*(1 + r)*(1 + t) - 0.5*(Hs[13] + Hs[14] + Hs[18]);
			Hs[7] =  0.125*(1 - r)*(1 + t) - 0.5*(Hs[14] + Hs[15] + Hs[19]);

			Ht[ 8] = -0.25*(1 - r*r)*(1 - s);
			Ht[ 9] = -0.25*(1 - s*s)*(1 + r);
			Ht[10] = -0.25*(1 - r*r)*(1 + s);
			Ht[11] = -0.25*(1 - s*s)*(1 - r);
			Ht[12] =  0.25*(1 - r*r)*(1 - s);
			Ht[13] =  0.25*(1 - s*s)*(1 + r);
			Ht[14] =  0.25*(1 - r*r)*(1 + s);
			Ht[15] =  0.25*(1 - s*s)*(1 - r);
			Ht[16] = -0.5*t*(1 - r)*(1 - s);
			Ht[17] = -0.5*t*(1 + r)*(1 - s);
			Ht[18] = -0.5*t*(1 + r)*(1 + s);
			Ht[19] = -0.5*t*(1 - r)*(1 + s);
				
			Ht[0] = -0.125*(1 - r)*(1 - s) - 0.5*(Ht[ 8] + Ht[11] + Ht[16]);
			Ht[1] = -0.125*(1 + r)*(1 - s) - 0.5*(Ht[ 8] + Ht[ 9] + Ht[17]);
			Ht[2] = -0.125*(1 + r)*(1 + s) - 0.5*(Ht[ 9] + Ht[10] + Ht[18]);
			Ht[3] = -0.125*(1 - r)*(1 + s) - 0.5*(Ht[10] + Ht[11] + Ht[19]);
			Ht[4] =  0.125*(1 - r)*(1 - s) - 0.5*(Ht[12] + Ht[15] + Ht[16]);
			Ht[5] =  0.125*(1 + r)*(1 - s) - 0.5*(Ht[12] + Ht[13] + Ht[17]);
			Ht[6] =  0.125*(1 + r)*(1 + s) - 0.5*(Ht[13] + Ht[14] + Ht[18]);
			Ht[7] =  0.125*(1 - r)*(1 + s) - 0.5*(Ht[14] + Ht[15] + Ht[19]);
		}
		break;
	case FE_HEX27:
		{
			double R[3] = {0.5*r*(r-1.0), 0.5*r*(r+1.0), 1.0 - r*r};
			double S[3] = {0.5*s*(s-1.0), 0.5*s*(s+1.0), 1.0 - s*s};
			double T[3] = {0.5*t*(t-1.0), 0.5*t*(t+1.0), 1.0 - t*t};

			double DR[3] = {r - 0.5, r  + 0.5, -2.0*r};
			double DS[3] = {s - 0.5, s  + 0.5, -2.0*s};
			double DT[3] = {t - 0.5, t  + 0.5, -2.0*t};

			Hr[ 0] = DR[0]*S[0]*T[0];
			Hr[ 1] = DR[1]*S[0]*T[0];
			Hr[ 2] = DR[1]*S[1]*T[0];
			Hr[ 3] = DR[0]*S[1]*T[0];
			Hr[ 4] = DR[0]*S[0]*T[1];
			Hr[ 5] = DR[1]*S[0]*T[1];
			Hr[ 6] = DR[1]*S[1]*T[1];
			Hr[ 7] = DR[0]*S[1]*T[1];
			Hr[ 8] = DR[2]*S[0]*T[0];
			Hr[ 9] = DR[1]*S[2]*T[0];
			Hr[10] = DR[2]*S[1]*T[0];
			Hr[11] = DR[0]*S[2]*T[0];
			Hr[12] = DR[2]*S[0]*T[1];
			Hr[13] = DR[1]*S[2]*T[1];
			Hr[14] = DR[2]*S[1]*T[1];
			Hr[15] = DR[0]*S[2]*T[1];
			Hr[16] = DR[0]*S[0]*T[2];
			Hr[17] = DR[1]*S[0]*T[2];
			Hr[18] = DR[1]*S[1]*T[2];
			Hr[19] = DR[0]*S[1]*T[2];
			Hr[20] = DR[2]*S[0]*T[2];
			Hr[21] = DR[1]*S[2]*T[2];
			Hr[22] = DR[2]*S[1]*T[2];
			Hr[23] = DR[0]*S[2]*T[2];
			Hr[24] = DR[2]*S[2]*T[0];
			Hr[25] = DR[2]*S[2]*T[1];
			Hr[26] = DR[2]*S[2]*T[2];

			Hs[ 0] = R[0]*DS[0]*T[0];
			Hs[ 1] = R[1]*DS[0]*T[0];
			Hs[ 2] = R[1]*DS[1]*T[0];
			Hs[ 3] = R[0]*DS[1]*T[0];
			Hs[ 4] = R[0]*DS[0]*T[1];
			Hs[ 5] = R[1]*DS[0]*T[1];
			Hs[ 6] = R[1]*DS[1]*T[1];
			Hs[ 7] = R[0]*DS[1]*T[1];
			Hs[ 8] = R[2]*DS[0]*T[0];
			Hs[ 9] = R[1]*DS[2]*T[0];
			Hs[10] = R[2]*DS[1]*T[0];
			Hs[11] = R[0]*DS[2]*T[0];
			Hs[12] = R[2]*DS[0]*T[1];
			Hs[13] = R[1]*DS[2]*T[1];
			Hs[14] = R[2]*DS[1]*T[1];
			Hs[15] = R[0]*DS[2]*T[1];
			Hs[16] = R[0]*DS[0]*T[2];
			Hs[17] = R[1]*DS[0]*T[2];
			Hs[18] = R[1]*DS[1]*T[2];
			Hs[19] = R[0]*DS[1]*T[2];
			Hs[20] = R[2]*DS[0]*T[2];
			Hs[21] = R[1]*DS[2]*T[2];
			Hs[22] = R[2]*DS[1]*T[2];
			Hs[23] = R[0]*DS[2]*T[2];
			Hs[24] = R[2]*DS[2]*T[0];
			Hs[25] = R[2]*DS[2]*T[1];
			Hs[26] = R[2]*DS[2]*T[2];

			Ht[ 0] = R[0]*S[0]*DT[0];
			Ht[ 1] = R[1]*S[0]*DT[0];
			Ht[ 2] = R[1]*S[1]*DT[0];
			Ht[ 3] = R[0]*S[1]*DT[0];
			Ht[ 4] = R[0]*S[0]*DT[1];
			Ht[ 5] = R[1]*S[0]*DT[1];
			Ht[ 6] = R[1]*S[1]*DT[1];
			Ht[ 7] = R[0]*S[1]*DT[1];
			Ht[ 8] = R[2]*S[0]*DT[0];
			Ht[ 9] = R[1]*S[2]*DT[0];
			Ht[10] = R[2]*S[1]*DT[0];
			Ht[11] = R[0]*S[2]*DT[0];
			Ht[12] = R[2]*S[0]*DT[1];
			Ht[13] = R[1]*S[2]*DT[1];
			Ht[14] = R[2]*S[1]*DT[1];
			Ht[15] = R[0]*S[2]*DT[1];
			Ht[16] = R[0]*S[0]*DT[2];
			Ht[17] = R[1]*S[0]*DT[2];
			Ht[18] = R[1]*S[1]*DT[2];
			Ht[19] = R[0]*S[1]*DT[2];
			Ht[20] = R[2]*S[0]*DT[2];
			Ht[21] = R[1]*S[2]*DT[2];
			Ht[22] = R[2]*S[1]*DT[2];
			Ht[23] = R[0]*S[2]*DT[2];
			Ht[24] = R[2]*S[2]*DT[0];
			Ht[25] = R[2]*S[2]*DT[1];
			Ht[26] = R[2]*S[2]*DT[2];
		}
		break;
	default:
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void FEElement::iso_coord(int n, double q[3])
{
    // for n=-1 return isoparametric coordinates of element center
    
	assert((n>=-1)&&(n<Nodes()));
	switch (m_ntype)
	{
	case FE_TET4:
		{
			switch (n)
			{
            case -1: q[0] = 0.25; q[1] = 0.25; q[2] = 0.25; break;
			case 0: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case 1: q[0] = 1; q[1] = 0; q[2] = 0; break;
			case 2: q[0] = 0; q[1] = 1; q[2] = 0; break;
			case 3: q[0] = 0; q[1] = 0; q[2] = 1; break;
			}
		}
		break;
	case FE_PENTA6:
		{
			const double t = 1.0/3.0;
			switch (n)
			{
            case -1: q[0] = t; q[1] = t; q[2] = 0; break;
			case 0: q[0] = 0; q[1] = 0; q[2] = -1; break;
			case 1: q[0] = 1; q[1] = 0; q[2] = -1; break;
			case 2: q[0] = 0; q[1] = 1; q[2] = -1; break;
			case 3: q[0] = 0; q[1] = 0; q[2] =  1; break;
			case 4: q[0] = 1; q[1] = 0; q[2] =  1; break;
			case 5: q[0] = 0; q[1] = 1; q[2] =  1; break;
			}
		}
		break;
	case FE_HEX8:
		{
			switch (n)
			{
            case -1: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case 0: q[0] = -1; q[1] = -1; q[2] = -1; break;
			case 1: q[0] =  1; q[1] = -1; q[2] = -1; break;
			case 2: q[0] =  1; q[1] =  1; q[2] = -1; break;
			case 3: q[0] = -1; q[1] =  1; q[2] = -1; break;
			case 4: q[0] = -1; q[1] = -1; q[2] =  1; break;
			case 5: q[0] =  1; q[1] = -1; q[2] =  1; break;
			case 6: q[0] =  1; q[1] =  1; q[2] =  1; break;
			case 7: q[0] = -1; q[1] =  1; q[2] =  1; break;
			}
		}
		break;
	case FE_TET10:
		{
			switch (n)
			{
            case -1: q[0] = 0.25; q[1] = 0.25; q[2] = 0.25; break;
			case 0: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case 1: q[0] = 1; q[1] = 0; q[2] = 0; break;
			case 2: q[0] = 0; q[1] = 1; q[2] = 0; break;
			case 3: q[0] = 0; q[1] = 0; q[2] = 1; break;
			case 4: q[0] = 0.5; q[1] = 0.0; q[2] = 0.0; break;
			case 5: q[0] = 0.5; q[1] = 0.5; q[2] = 0.0; break;
			case 6: q[0] = 0.0; q[1] = 0.5; q[2] = 0.0; break;
			case 7: q[0] = 0.0; q[1] = 0.0; q[2] = 0.5; break;
			case 8: q[0] = 0.5; q[1] = 0.0; q[2] = 0.5; break;
			case 9: q[0] = 0.0; q[1] = 0.5; q[2] = 0.5; break;
			}
		}
		break;
	case FE_TET15:
		{
			const double t = 1.0/3.0;
			switch (n)
			{
            case -1: q[0] = 0.25; q[1] = 0.25; q[2] = 0.25; break;
			case 0: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case 1: q[0] = 1; q[1] = 0; q[2] = 0; break;
			case 2: q[0] = 0; q[1] = 1; q[2] = 0; break;
			case 3: q[0] = 0; q[1] = 0; q[2] = 1; break;
			case 4: q[0] = 0.5; q[1] = 0.0; q[2] = 0.0; break;
			case 5: q[0] = 0.5; q[1] = 0.5; q[2] = 0.0; break;
			case 6: q[0] = 0.0; q[1] = 0.5; q[2] = 0.0; break;
			case 7: q[0] = 0.0; q[1] = 0.0; q[2] = 0.5; break;
			case 8: q[0] = 0.5; q[1] = 0.0; q[2] = 0.5; break;
			case 9: q[0] = 0.0; q[1] = 0.5; q[2] = 0.5; break;
			case 10: q[0] = t; q[1] = t; q[2] = 0; break;
			case 11: q[0] = t; q[1] = 0; q[2] = t; break;
			case 12: q[0] = t; q[1] = t; q[2] = t; break;
			case 13: q[0] = 0; q[1] = t; q[2] = t; break;
			case 14: q[0] = 0.25; q[1] = 0.25; q[2] = 0.25; break;
			}
		}
		break;
	case FE_TET20:
		{
			// TODO: Implement this
			assert(false);
		}
		break;
	case FE_HEX20:
		{
			switch (n)
			{
            case -1: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case  0: q[0] = -1; q[1] = -1; q[2] = -1; break;
			case  1: q[0] =  1; q[1] = -1; q[2] = -1; break;
			case  2: q[0] =  1; q[1] =  1; q[2] = -1; break;
			case  3: q[0] = -1; q[1] =  1; q[2] = -1; break;
			case  4: q[0] = -1; q[1] = -1; q[2] =  1; break;
			case  5: q[0] =  1; q[1] = -1; q[2] =  1; break;
			case  6: q[0] =  1; q[1] =  1; q[2] =  1; break;
			case  7: q[0] = -1; q[1] =  1; q[2] =  1; break;
			case  8: q[0] =  0; q[1] = -1; q[2] = -1; break;
			case  9: q[0] =  1; q[1] =  0; q[2] = -1; break;
			case 10: q[0] =  0; q[1] =  1; q[2] = -1; break;
			case 11: q[0] = -1; q[1] =  0; q[2] = -1; break;
			case 12: q[0] =  0; q[1] = -1; q[2] =  1; break;
			case 13: q[0] =  1; q[1] =  0; q[2] =  1; break;
			case 14: q[0] =  0; q[1] =  1; q[2] =  1; break;
			case 15: q[0] = -1; q[1] =  0; q[2] =  1; break;
			case 16: q[0] = -1; q[1] = -1; q[2] =  0; break;
			case 17: q[0] =  1; q[1] = -1; q[2] =  0; break;
			case 18: q[0] =  1; q[1] =  1; q[2] =  0; break;
			case 19: q[0] = -1; q[1] =  1; q[2] =  0; break;
			}
		}
		break;
	case FE_HEX27:
		{
			switch (n)
			{
            case -1: q[0] = 0; q[1] = 0; q[2] = 0; break;
			case  0: q[0] = -1; q[1] = -1; q[2] = -1; break;
			case  1: q[0] =  1; q[1] = -1; q[2] = -1; break;
			case  2: q[0] =  1; q[1] =  1; q[2] = -1; break;
			case  3: q[0] = -1; q[1] =  1; q[2] = -1; break;
			case  4: q[0] = -1; q[1] = -1; q[2] =  1; break;
			case  5: q[0] =  1; q[1] = -1; q[2] =  1; break;
			case  6: q[0] =  1; q[1] =  1; q[2] =  1; break;
			case  7: q[0] = -1; q[1] =  1; q[2] =  1; break;
			case  8: q[0] =  0; q[1] = -1; q[2] = -1; break;
			case  9: q[0] =  1; q[1] =  0; q[2] = -1; break;
			case 10: q[0] =  0; q[1] =  1; q[2] = -1; break;
			case 11: q[0] = -1; q[1] =  0; q[2] = -1; break;
			case 12: q[0] =  0; q[1] = -1; q[2] =  1; break;
			case 13: q[0] =  1; q[1] =  0; q[2] =  1; break;
			case 14: q[0] =  0; q[1] =  1; q[2] =  1; break;
			case 15: q[0] = -1; q[1] =  0; q[2] =  1; break;
			case 16: q[0] = -1; q[1] = -1; q[2] =  0; break;
			case 17: q[0] =  1; q[1] = -1; q[2] =  0; break;
			case 18: q[0] =  1; q[1] =  1; q[2] =  0; break;
			case 19: q[0] = -1; q[1] =  1; q[2] =  0; break;
			case 20: q[0] =  0; q[1] = -1; q[2] =  0; break;
			case 21: q[0] =  1; q[1] =  0; q[2] =  0; break;
			case 22: q[0] =  0; q[1] =  1; q[2] =  0; break;
			case 23: q[0] = -1; q[1] =  0; q[2] =  0; break;
			case 24: q[0] =  0; q[1] =  0; q[2] = -1; break;
			case 25: q[0] =  0; q[1] =  0; q[2] =  1; break;
			case 26: q[0] =  0; q[1] =  0; q[2] =  0; break;
			}
		}
		break;
	}
}

