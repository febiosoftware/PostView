// FEElement.h: interface for the FEElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEELEMENT_H__F82D0D55_F582_4ABE_9D02_C9FC5FC72C6C__INCLUDED_)
#define AFX_FEELEMENT_H__F82D0D55_F582_4ABE_9D02_C9FC5FC72C6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math3d.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// Different element types (do not change the order of these types since the
// type is used as an index in some of the element functions.)
enum FEElemType {
	FE_HEX8,
	FE_TET4,
	FE_PENTA6,
	FE_QUAD4,
	FE_TRI3,
	FE_TRUSS2,
	FE_HEX20,
	FE_QUAD8,
	FE_TRUSS3,
	FE_TET10,
	FE_TET15,
	FE_HEX27,
    FE_TRI6,
    FE_QUAD9
};

//-----------------------------------------------------------------------------
// Different face types (do not change the order)
enum FEFaceType {
	FACE_TRI3,
	FACE_QUAD4,
	FACE_QUAD8,
	FACE_TRI6,
	FACE_TRI7,
	FACE_QUAD9
};

//-----------------------------------------------------------------------------
// Different edge types
enum FEEdgeType {
	EDGE_LINE2,
	EDGE_LINE3
};

//-----------------------------------------------------------------------------
// FE State Flags
#define FE_VISIBLE	0x01		// render the item ?
#define FE_SELECTED	0x02		// is the item currently selected ?
#define FE_ENABLED	0x04		// should the item be evaluated ?
#define FE_ACTIVE	0x08		// does the item contain data?

//-----------------------------------------------------------------------------
// Forward declaration of the element class
class FEElement;

//-----------------------------------------------------------------------------
// base class for mesh item classes.
//
class FEItem
{
public:
	FEItem() { m_state = FE_VISIBLE | FE_ENABLED; }
	virtual ~FEItem() {}

	bool IsVisible () const { return ((m_state & FE_VISIBLE ) != 0); }
	bool IsSelected() const { return ((m_state & FE_SELECTED) != 0); }
	bool IsEnabled () const { return ((m_state & FE_ENABLED ) != 0); }
	bool IsActive  () const { return ((m_state & FE_ACTIVE  ) != 0); }

	void Select()   { m_state = m_state | FE_SELECTED; }
	void Unselect() { m_state = m_state & ~FE_SELECTED; }

	void Show  () { m_state = m_state | FE_VISIBLE;  }
	void Hide  () { m_state = m_state & ~(FE_VISIBLE | FE_SELECTED);  }

	void Enable() { m_state = m_state | FE_ENABLED; }
	void Disable() { m_state = m_state & ~FE_ENABLED; }

	void Activate  () { m_state = m_state | FE_ACTIVE; }
	void Deactivate() { m_state = m_state & ~FE_ACTIVE; }

	unsigned int GetFEState() const { return m_state; }
	void SetFEState(unsigned int state) { m_state = state; }

public:
	int	m_ntag;

public:
	unsigned int m_state;	// the state flag of the mesh(-item)
};

//-----------------------------------------------------------------------------
// Class describing a node of the mesh
class FENode : public FEItem
{
public:
	FENode() { m_tex = 0.f; }

public:
	vec3f	m_r0;	// initial coordinates of node // TODO: I would like to remove this variable
	vec3f	m_rt;	// current coordinates of node
	bool	m_bext;	// interior or exterior node
	float	m_tex;	// nodal texture coordinate
};

//-----------------------------------------------------------------------------
// Class describing an edge of the mesh. The edges identify the smooth boundaries
class FEEdge : public FEItem
{
public:
	enum {MAX_NODES = 3};

	FEEdgeType Type() const { return m_type; }

public:
	int node[MAX_NODES];
	FEEdgeType	m_type;

public:
	FEEdge();

	bool operator == (const FEEdge& e)
	{
		if ((node[0] == e.node[0]) && (node[1] == e.node[1])) return true;
		if ((node[0] == e.node[1]) && (node[1] == e.node[0])) return true;
		return false;
	}

	int Nodes() const
	{
		const int N[] = {2,3}; 
		return N[m_type];
	}

	// evaluate shape function at iso-parameteric point (r,s)
	void shape(double* H, double r);

	// evaluate a vector expression at iso-points (r,s)
	double eval(double* d, double r);

	// evaluate a vector expression at iso-points (r,s)
	vec3f eval(vec3f* v, double r);
};

//-----------------------------------------------------------------------------
// Class that describes an exterior face of the mesh
class FEFace : public FEItem
{
public:
	enum { MAX_NODES = 9 };

public:
	int	node[MAX_NODES];	// array of indices to the four nodes of a face
	int	m_ntype;			// type of face

	int	m_nsg;				// smoothing group ID
	int	m_mat;				// material id
	int	m_nbr[4];			// neighbour faces
	int	m_elem[2];			// first index = element to which this face belongs, second index = local element face number

	vec3f	m_fn;				// face normal
	vec3f	m_nn[MAX_NODES];	// node normals
	float	m_tex[MAX_NODES];	// nodal 1D-texture coordinates
	float	m_texe;				// element texture coordinate

public:
	FEFace();

	bool operator == (const FEFace& face)
	{
		const int* pn = face.node;
		if (m_ntype != face.m_ntype) return false;
		switch (m_ntype)
		{
		case FACE_TRI3:
		case FACE_TRI6:
		case FACE_TRI7:
			if ((pn[0] != node[0]) && (pn[0] != node[1]) && (pn[0] != node[2])) return false;
			if ((pn[1] != node[0]) && (pn[1] != node[1]) && (pn[1] != node[2])) return false;
			if ((pn[2] != node[0]) && (pn[2] != node[1]) && (pn[2] != node[2])) return false;
			break;
		case FACE_QUAD4:
		case FACE_QUAD8:
		case FACE_QUAD9:
			if ((pn[0] != node[0]) && (pn[0] != node[1]) && (pn[0] != node[2]) && (pn[0] != node[3])) return false;
			if ((pn[1] != node[0]) && (pn[1] != node[1]) && (pn[1] != node[2]) && (pn[1] != node[3])) return false;
			if ((pn[2] != node[0]) && (pn[2] != node[1]) && (pn[2] != node[2]) && (pn[2] != node[3])) return false;
			if ((pn[3] != node[0]) && (pn[3] != node[1]) && (pn[3] != node[2]) && (pn[3] != node[3])) return false;
			break;
		}
		return true;
	}

	bool HasEdge(int n1, int n2)
	{
		switch (m_ntype)
		{
		case FACE_TRI3:
		case FACE_TRI6:
		case FACE_TRI7:
			if (((node[0]==n1) && (node[1]==n2)) || ((node[1]==n1) && (node[0]==n2))) return true;
			if (((node[1]==n1) && (node[2]==n2)) || ((node[2]==n1) && (node[1]==n2))) return true;
			if (((node[2]==n1) && (node[0]==n2)) || ((node[0]==n1) && (node[2]==n2))) return true;
			break;
		case FACE_QUAD4:
		case FACE_QUAD8:
		case FACE_QUAD9:
			if (((node[0]==n1) && (node[1]==n2)) || ((node[1]==n1) && (node[0]==n2))) return true;
			if (((node[1]==n1) && (node[2]==n2)) || ((node[2]==n1) && (node[1]==n2))) return true;
			if (((node[2]==n1) && (node[3]==n2)) || ((node[3]==n1) && (node[2]==n2))) return true;
			if (((node[3]==n1) && (node[0]==n2)) || ((node[0]==n1) && (node[3]==n2))) return true;
			break;
		}
		return false;
	}

	bool HasNode(int n)
	{
		int N = Nodes();
		for (int i=0; i<N; ++i) if (node[i] == n) return true;
		return false;
	}

	int Nodes() 
	{
		const int n[6] = {3, 4, 8, 6, 7, 9};
		assert((m_ntype >= 0) && (m_ntype <= 5));
		return n[m_ntype]; 
	}

	int Edges()
	{
		const int n[6] = {3, 4, 4, 3, 3, 4};
		assert((m_ntype >= 0) && (m_ntype <= 5));
		return n[m_ntype]; 
	}

	FEEdge Edge(int i);

	// evaluate shape function at iso-parameteric point (r,s)
	void shape(double* H, double r, double s);

	// evaluate a vector expression at iso-points (r,s)
	double eval(double* d, double r, double s);

	// evaluate a vector expression at iso-points (r,s)
	vec3f eval(vec3f* v, double r, double s);
};

//-----------------------------------------------------------------------------
// Class that describes an element of the mesh
class FEElement : public FEItem
{
public:
	enum {MAX_NODES = 27};

public:
	FEElement();
	virtual ~FEElement() {}
		
	bool HasNode(int node) 
	{ 
		bool ret = false;
		for (int i=0; i<Nodes(); i++) ret |= !(m_node[i] ^ node);
				
		return ret;
	}

	int Faces() const
	{
		const int fc[14] = {6, 4, 5, 0, 0, 0, 6, 0, 0, 4, 4, 6, 0, 0};
		return fc[m_ntype];
	}

	int Edges() const
	{
		const int ec[14] = {0, 0, 0, 4, 3, 0, 0, 4, 0, 0, 0, 0, 3, 4};
		return ec[m_ntype];
	}

	int Nodes() const
	{
		const int nc[14] = {8, 4, 6, 4, 3, 2, 20, 8, 3, 10, 15, 27, 6, 9};
		return nc[m_ntype];
	}

	FEFace GetFace(int i) const;
	FEEdge GetEdge(int i) const;

	bool IsSolid() { return ((m_ntype == FE_HEX8  ) || (m_ntype == FE_HEX20 ) || (m_ntype == FE_TET4) || (m_ntype == FE_PENTA6) || (m_ntype == FE_TET10) || (m_ntype == FE_TET15) || (m_ntype == FE_HEX27)); }
	bool IsShell() { return ((m_ntype == FE_QUAD4 ) || (m_ntype == FE_QUAD8 ) || (m_ntype == FE_QUAD9 ) || (m_ntype == FE_TRI3) || (m_ntype == FE_TRI6)); }
	bool IsBeam () { return ((m_ntype == FE_TRUSS2) || (m_ntype == FE_TRUSS3)); }

	bool operator != (FEElement& el);

	// evaluate shape function at iso-parameteric point (r,s,t)
	void shape(double* H, double r, double s, double t);

	// evaluate a vector expression at iso-points (r,s)
	double eval(double* d, double r, double s, double t);

	// evaluate a vector expression at iso-points (r,s)
	vec3f eval(vec3f* v, double r, double s, double t);

	// shape function derivatives
	void shape_deriv(double* Hr, double* Hs, double* Ht, double r, double s, double t);

	// get iso-param coordinates of the nodes
	void iso_coord(int n, double q[3]);

public:
	FEElemType	m_ntype;			// type of element
	int			m_nId;				// id of this element
	int			m_lid;				// local ID (zero-based index into element array)
	int			m_MatID;			// material id
	int			m_node[MAX_NODES];	// array of nodes ID

	FEElement*	m_pElem[6];		// array of pointers to neighbour elements
};

#endif // !defined(AFX_FEELEMENT_H__F82D0D55_F582_4ABE_9D02_C9FC5FC72C6C__INCLUDED_)
