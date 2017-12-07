// FEMesh.h: interface for the FEMeshBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEMESH_H__4E540300_07D8_4732_BB8D_6570BB162180__INCLUDED_)
#define AFX_FEMESH_H__4E540300_07D8_4732_BB8D_6570BB162180__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FEElement.h"
#include "FEGroup.h"
#include "FENodeElemList.h"
#include "FENodeFaceList.h"
#include "bbox.h"
#include <utility>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class FEMeshBase
{
public:
	// --- M E M O R Y   M A N A G M E N T ---
	//! constructor
	FEMeshBase();

	//! destructor
	virtual ~FEMeshBase();

	//! allocate storage for mesh
	void Create(int nodes, int elems);

	//! Clean up all data
	void CleanUp();

	//! clean mesh and all data
	void ClearAll();

	vector<NodeElemRef>& NodeElemList(int n) { return m_NEL.ElemList(n); }
	vector<NodeFaceRef>& NodeFaceList(int n) { return m_NFL.FaceList(n); }

	// --- G E O M E T R Y ---
	//! return nr of nodes
	int Nodes() const { return (int) m_Node.size(); }

	//! return nr of edges
	int Edges() const { return (int) m_Edge.size(); }

	//! return nr of faces
	int Faces() const { return (int) m_Face.size(); }

	//! return nr of elements
	virtual int Elements() const = 0;

	virtual void ClearElements() = 0;

	//! return nr of shell elements
	int ShellElements();

	//! return nr of solid elements
	int SolidElements();

	//! return nr of beam elements
	int BeamElements();

	//! return a node
	FENode& Node(int i) { return m_Node[i]; }
	const FENode& Node(int i) const { return m_Node[i]; }

	//! return an edge
	FEEdge& Edge(int i) { return m_Edge[i]; }
	const FEEdge& Edge(int i) const { return m_Edge[i]; }

	//! return a face
	FEFace& Face(int i) { return m_Face[i]; }
	const FEFace& Face(int i) const { return m_Face[i]; }

	//! return an element
	virtual FEElement& Element(int i) = 0;
	virtual const FEElement& Element(int i) const = 0;

	//! return domains
	int Domains() const { return (int) m_Dom.size(); }

	//! return a domain
	FEDomain& Domain(int i) { return *m_Dom[i]; }

	//! nr of parts
	int Parts() const { return (int) m_Part.size(); }

	//! add a part
	void AddPart(FEPart* pg) { m_Part.push_back(pg); }

	//! return a part
	FEPart& Part(int n) { return *m_Part[n]; }

	// number of surfaces
	int Surfaces() const { return (int) m_Surf.size(); }

	// return a surface
	FESurface& Surface(int n) { return *m_Surf[n]; }

	// Add a surface
	void AddSurface(FESurface* ps) { m_Surf.push_back(ps); }

	//! number of node sets
	int NodeSets() const { return (int) m_NSet.size(); }

	//! return a node set
	FENodeSet& NodeSet(int i) { return *m_NSet[i]; }

	//! Add a node set
	void AddNodeSet(FENodeSet* ps) { m_NSet.push_back(ps); }

	// --- D A T A   U P D A T E ---

	//! update mesh data
	void Update();

	//! update the normals of the mesh
	void UpdateNormals(bool smooth);

	// --- E V A L U A T E ---

	vec3f ElementCenter(FEElement& el)
	{
		vec3f r;
		int N = el.Nodes();
		for (int i=0; i<N; i++) r += m_Node[el.m_node[i]].m_rt;
		return r/(float)N;
	}

	vec3f FaceCenter(FEFace& f)
	{
		vec3f r;
		int N = f.Nodes();
		for (int i=0; i<N; i++) r += m_Node[f.node[i]].m_rt;
		return r/(float)N;
	}

	vec3f EdgeCenter(FEEdge& e)
	{
		return (m_Node[e.node[0]].m_rt + m_Node[e.node[1]].m_rt)*0.5f;
	}

	// face area
	double FaceArea(FEFace& f);

	// element volume
	float ElementVolume(int iel);
	float HexVolume    (const FEElement& el);
	float PentaVolume  (const FEElement& el);
	float TetVolume    (const FEElement& el);
	float PyramidVolume(const FEElement& el);

	// --- I N T E G R A T E ---
	float IntegrateQuad(vec3f* r, float* v);
	float IntegrateHex (vec3f* r, float* v);

	// --- F A C E   D A T A ---
	void FaceNodePosition (const FEFace& f, vec3f* r) const;
	void FaceNodeNormals  (FEFace& f, vec3f* n);
	void FaceNodeTexCoords(FEFace& f, float* t, bool bnode);

	void AutoSmooth(double angleRadians);

	// --- S E L E C T I O N ---
	int CountSelectedFaces() const;

	void SetNodeTags(int ntag);
	void SetEdgeTags(int ntag);
	void SetFaceTags(int ntag);
	void SetElementTags(int ntag);

protected:
	virtual void CreateElements(int elems) = 0;

protected:
	void BuildFaces();
	void FindFaceNeighbors();
	void FindNeighbours();
	void UpdateNodes();
	void BuildEdges();
	void UpdateDomains();

	void ClearDomains();
	void ClearParts();
	void ClearSurfaces();
	void ClearNodeSets();

protected:
	// --- G E O M E T R Y ---
	vector<FENode>		m_Node;	// nodal array
	vector<FEEdge>		m_Edge;	// edge array
	vector<FEFace>		m_Face;	// face array
	vector<FEDomain*>	m_Dom;	// domains

	// user-defined partitions
	vector<FEPart*>		m_Part;	// parts
	vector<FESurface*>	m_Surf;	// surfaces
	vector<FENodeSet*>	m_NSet;	// node sets

	FENodeElemList		m_NEL;
	FENodeFaceList		m_NFL;

	int		m_nID;
};

template <class T> class FEMesh_ : public FEMeshBase
{
public:
	FEMesh_(){}

public:
	// number of elements
	int Elements() const { return (int) m_Elem.size(); }

	//! return an element
	FEElement& Element(int i) { return m_Elem[i]; }
	const FEElement& Element(int i) const { return m_Elem[i]; }

	void ClearElements() { m_Elem.clear(); }

protected:
	void CreateElements(int elems) { m_Elem.resize(elems); }

protected:
	vector<T>	m_Elem;	// element array
};

typedef FEMesh_<FETri3> FETriMesh;
typedef FEMesh_<FEQuad4> FEQuadMesh;
typedef FEMesh_<FETet4> FEMeshTet4;
typedef FEMesh_<FEHex8> FEMeshHex8;
typedef FEMesh_<FEGenericElement> FEMesh;
typedef FEMesh_<FELinearElement> FELinearMesh;

// find the element and the iso-parametric coordinates of a point inside the mesh
bool FindElementRef(FEMeshBase& m, const vec3f& x, int& nelem, double r[3]);

// find the element and the iso-parametric coordinates of a point inside the mesh
// the x coordinates is assumed to be in reference frame
bool FindElementInReferenceFrame(FEMeshBase& m, const vec3f& x, int& nelem, double r[3]);

class FEFindElement
{
public:
	FEFindElement(FEMeshBase& mesh);

	bool FindInReferenceFrame(const vec3f& x, int& nelem, double r[3]);

private:
	FEMeshBase&	m_mesh;
	vector<BOUNDINGBOX>	m_box;
};

#endif // !defined(AFX_FEMESH_H__4E540300_07D8_4732_BB8D_6570BB162180__INCLUDED_)
