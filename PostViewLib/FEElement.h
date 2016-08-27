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
// Different element types
enum FEElemType {
	FE_LINE2,
	FE_LINE3,
	FE_TRI3,
    FE_TRI6,
	FE_QUAD4,
	FE_QUAD8,
    FE_QUAD9,
	FE_TET4,
	FE_TET10,
	FE_TET15,
	FE_PENTA6,
	FE_HEX8,
	FE_HEX20,
	FE_HEX27
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
#define FE_HIDDEN		0x01		// was the item hidden by the user
#define FE_SELECTED		0x02		// is the item currently selected ?
#define FE_DISABLED		0x04		// should the item be evaluated ?
#define FE_ACTIVE		0x08		// does the item contain data?
#define FE_INVISIBLE	0x10		// is the item actually visible? 
                                    // Even when not hidden, the item may not be shown since e.g. the material is hidden

//-----------------------------------------------------------------------------
// Forward declaration of the element class
class FEElement;

//-----------------------------------------------------------------------------
// base class for mesh item classes.
//
class FEItem
{
public:
	FEItem() { m_state = 0; m_nId = -1; }
	virtual ~FEItem() {}

	bool IsHidden   () const { return ((m_state & FE_HIDDEN   ) != 0); }
	bool IsSelected () const { return ((m_state & FE_SELECTED ) != 0); }
	bool IsDisabled () const { return ((m_state & FE_DISABLED ) != 0); }
	bool IsActive   () const { return ((m_state & FE_ACTIVE   ) != 0); }
	bool IsInvisible() const { return ((m_state & FE_INVISIBLE) != 0); }

	void Select  () { m_state = m_state |  FE_SELECTED; }
	void Unselect() { m_state = m_state & ~FE_SELECTED; }

	void Hide  () { m_state = (m_state | FE_HIDDEN) & ~FE_SELECTED;  }
	void Unhide() { m_state = m_state & ~FE_HIDDEN;  }

	void Enable () { m_state = m_state & ~FE_DISABLED; }
	void Disable() { m_state = m_state | FE_DISABLED; }

	void Activate  () { m_state = m_state | FE_ACTIVE; }
	void Deactivate() { m_state = m_state & ~FE_ACTIVE; }

	bool IsEnabled() const { return (IsDisabled() == false); }

	void Show(bool bshow)
	{
		if (bshow) m_state = m_state & ~FE_INVISIBLE;
		else m_state = (m_state | FE_INVISIBLE) & ~FE_SELECTED; 
	}

	bool IsVisible() const { return (IsInvisible() == false) && (IsHidden() == false); }

	unsigned int GetFEState() const { return m_state; }
	void SetFEState(unsigned int state) { m_state = state; }

	int GetID() const { return m_nId; }
	void SetID(int nid) { m_nId = nid; }

public:
	int	m_ntag;
	int	m_nId;

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
	FEElement();
		
	bool HasNode(int node) const;

public:
	// derived classes must implement these
	virtual int Faces() const = 0;
	virtual int Edges() const = 0;
	virtual int Nodes() const = 0;

public:
	FEFace GetFace(int i) const;
	void GetFace(int i, FEFace& face) const;
	FEEdge GetEdge(int i) const;

	bool IsSolid() { return ((m_ntype == FE_HEX8  ) || (m_ntype == FE_HEX20 ) || (m_ntype == FE_TET4) || (m_ntype == FE_PENTA6) || (m_ntype == FE_TET10) || (m_ntype == FE_TET15) || (m_ntype == FE_HEX27)); }
	bool IsShell() { return ((m_ntype == FE_QUAD4 ) || (m_ntype == FE_QUAD8 ) || (m_ntype == FE_QUAD9 ) || (m_ntype == FE_TRI3) || (m_ntype == FE_TRI6)); }
	bool IsBeam () { return ((m_ntype == FE_LINE2) || (m_ntype == FE_LINE3)); }

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

	FEElemType Type() const { return m_ntype; }

public:
	int			m_lid;		// local ID (zero-based index into element array)
	int			m_MatID;	// material id
	int*		m_node;		// array of nodes ID

	FEElement*	m_pElem[6];		// array of pointers to neighbour elements

protected:
	FEElemType	m_ntype;			// type of element
};

//=============================================================================
// Element traits classes
template <int Type> class FEElementTraits {};
// Each of these classes defines:
// Nodes: number of nodes
// Faces: number of faces. Only solid elements define faces
// Edges: number of edges. Only surface elements define edges

template <> class FEElementTraits<FE_LINE2 >{ public: enum {Nodes =  2}; enum {Faces = 0}; enum {Edges = 0}; static FEElemType Type() { return FE_LINE2 ; }};
template <> class FEElementTraits<FE_LINE3 >{ public: enum {Nodes =  3}; enum {Faces = 0}; enum {Edges = 0}; static FEElemType Type() { return FE_LINE3 ; }};
template <> class FEElementTraits<FE_TRI3  >{ public: enum {Nodes =  3}; enum {Faces = 0}; enum {Edges = 3}; static FEElemType Type() { return FE_TRI3  ; }};
template <> class FEElementTraits<FE_TRI6  >{ public: enum {Nodes =  6}; enum {Faces = 0}; enum {Edges = 3}; static FEElemType Type() { return FE_TRI6  ; }};
template <> class FEElementTraits<FE_QUAD4 >{ public: enum {Nodes =  4}; enum {Faces = 0}; enum {Edges = 4}; static FEElemType Type() { return FE_QUAD4 ; }};
template <> class FEElementTraits<FE_QUAD8 >{ public: enum {Nodes =  8}; enum {Faces = 0}; enum {Edges = 4}; static FEElemType Type() { return FE_QUAD8 ; }};
template <> class FEElementTraits<FE_QUAD9 >{ public: enum {Nodes =  9}; enum {Faces = 0}; enum {Edges = 4}; static FEElemType Type() { return FE_QUAD9 ; }};
template <> class FEElementTraits<FE_TET4  >{ public: enum {Nodes =  4}; enum {Faces = 4}; enum {Edges = 0}; static FEElemType Type() { return FE_TET4  ; }};
template <> class FEElementTraits<FE_TET10 >{ public: enum {Nodes = 10}; enum {Faces = 4}; enum {Edges = 0}; static FEElemType Type() { return FE_TET10 ; }};
template <> class FEElementTraits<FE_TET15 >{ public: enum {Nodes = 15}; enum {Faces = 4}; enum {Edges = 0}; static FEElemType Type() { return FE_TET15 ; }};
template <> class FEElementTraits<FE_PENTA6>{ public: enum {Nodes =  6}; enum {Faces = 5}; enum {Edges = 0}; static FEElemType Type() { return FE_PENTA6; }};
template <> class FEElementTraits<FE_HEX8  >{ public: enum {Nodes =  8}; enum {Faces = 6}; enum {Edges = 0}; static FEElemType Type() { return FE_HEX8  ; }};
template <> class FEElementTraits<FE_HEX20 >{ public: enum {Nodes = 20}; enum {Faces = 6}; enum {Edges = 0}; static FEElemType Type() { return FE_HEX20 ; }};
template <> class FEElementTraits<FE_HEX27 >{ public: enum {Nodes = 27}; enum {Faces = 6}; enum {Edges = 0}; static FEElemType Type() { return FE_HEX27 ; }};

template <class T> class FEElementBase : public FEElement
{
public:
	FEElementBase()
	{
		m_ntype = T::Type();
		m_node = _node;
		for (int i=0; i<T::Nodes; ++i) m_node[i] = -1;
	}

	FEElementBase(const FEElementBase& el) : FEElement(el)
	{
		m_node = _node;
		for (int i=0; i<T::Nodes; ++i) m_node[i] = el.m_node[i];
	}

	void operator = (const FEElementBase& el)
	{
		FEElement::operator = (el);
		for (int i=0; i<T::Nodes; ++i) m_node[i] = el.m_node[i];
	}

public:
	int Nodes() const { return T::Nodes; }
	int Faces() const { return T::Faces; }
	int Edges() const { return T::Edges; }

public:
	int	_node[T::Nodes];
};

typedef FEElementBase< FEElementTraits<FE_LINE2 > > FELine2;
typedef FEElementBase< FEElementTraits<FE_LINE3 > > FELine3;
typedef FEElementBase< FEElementTraits<FE_TRI3  > > FETri3;
typedef FEElementBase< FEElementTraits<FE_TRI6  > > FETri6;
typedef FEElementBase< FEElementTraits<FE_QUAD4 > > FEQuad4;
typedef FEElementBase< FEElementTraits<FE_QUAD8 > > FEQuad8;
typedef FEElementBase< FEElementTraits<FE_QUAD9 > > FEQuad9;
typedef FEElementBase< FEElementTraits<FE_TET4  > > FETet4;
typedef FEElementBase< FEElementTraits<FE_TET10 > > FETet10;
typedef FEElementBase< FEElementTraits<FE_TET15 > > FETet15;
typedef FEElementBase< FEElementTraits<FE_PENTA6> > FEPenta6;
typedef FEElementBase< FEElementTraits<FE_HEX8  > > FEHex8;
typedef FEElementBase< FEElementTraits<FE_HEX20 > > FEHex20;
typedef FEElementBase< FEElementTraits<FE_HEX27 > > FEHex27;

//-----------------------------------------------------------------------------
// Generice element class that can represent any of the supported element classes
class FEGenericElement : public FEElement
{
public:
	enum {MAX_NODES = 27};

public:
	FEGenericElement();
	FEGenericElement(const FEGenericElement& e);
	void operator = (const FEGenericElement& e);

	void SetType(FEElemType type);

	int Nodes() const { return m_nodes; }
	int Faces() const { return m_faces; }
	int Edges() const { return m_edges; }

public:
	int		m_nodes, m_faces, m_edges;
	int		_node[MAX_NODES];	// array of nodes ID
};

//-----------------------------------------------------------------------------
// This element class can represent any of the linear elements.
class FELinearElement : public FEElement
{
public:
	enum {MAX_NODES = 8};

public:
	FELinearElement();
	FELinearElement(const FELinearElement& e);
	void operator = (const FELinearElement& e);

	void SetType(FEElemType type);

	int Nodes() const { return m_nodes; }
	int Faces() const { return m_faces; }
	int Edges() const { return m_edges; }

public:
	int		m_nodes, m_faces, m_edges;
	int		_node[MAX_NODES];	// array of nodes ID
};

#endif // !defined(AFX_FEELEMENT_H__F82D0D55_F582_4ABE_9D02_C9FC5FC72C6C__INCLUDED_)
