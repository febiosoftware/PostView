// FEMesh.h: interface for the FEMesh class.
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
#include <utility>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
class FEMesh
{
public:
	// --- M E M O R Y   M A N A G M E N T ---
	//! constructor
	FEMesh();

	//! destructor
	virtual ~FEMesh();

	//! allocate storage for mesh
	bool Create(int nodes, int elems);

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
	int Elements() const { return (int) m_Elem.size(); }

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
	FEElement& Element(int i) { return m_Elem[i]; }
	const FEElement& Element(int i) const { return m_Elem[i]; }

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

	//! set the smoothing angle
	void SetSmoothingAngle(double w) { m_stol = w*PI/180.0; }

	//! get the smoothing angle in degrees
	double GetSmoothingAngle() { return 180*m_stol/PI; }

	//! get the smoothing angle in radians
	double GetSmoothingAngleRadians() { return m_stol; }

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
	float HexVolume  (FEElement& el);
	float PentaVolume(FEElement& el);
	float TetVolume  (FEElement& el);

	// --- I N T E G R A T E ---
	float IntegrateQuad(vec3f* r, float* v);
	float IntegrateHex (vec3f* r, float* v);

	// --- S E L E C T I O N ---
	//TODO: Move selection stuff out of here and into a "FESelection" class.
	//! clear selection
	void ClearSelection();

	//! count the nr of selected items
	int CountSelectedNodes() const;
	int CountSelectedEdges() const;
	int CountSelectedElems() const;
	int CountSelectedFaces() const;

	//! select nodes in list
	void SelectNodes(vector<int>& item, bool bclear = true);

	//! select edges in list
	void SelectEdges(vector<int>& item, bool bclear = true);

	//! select faces in list
	void SelectFaces(vector<int>& item, bool bclear = true);

	//! select elements in list
	void SelectElements(vector<int>& item, bool bclear = true);

	//! select connected elements (connected via surface)
	void SelectConnectedSurfaceElements(FEElement& el);

	//! select connected elements (connected via volume)
	void SelectConnectedVolumeElements(FEElement& el);

	//! select connected faces
	void SelectConnectedFaces(FEFace& f);

	//! select connected edges
	void SelectConnectedEdges(FEEdge& e);

	//! select connected nodes on surface
	void SelectConnectedSurfaceNodes(int n);

	//! select connected nodes in volume
	void SelectConnectedVolumeNodes(int n);


	// --- V I S I B I L I T Y ---
	//! hide elements by material ID
	void HideElements(int nmat);

	//! show elements by material ID
	void ShowElements(int nmat);

	//! enable elements by material ID
	void EnableElements(int nmat);

	//! disable elements by material ID
	void DisableElements(int nmat);

	//! hide selected elements
	void HideSelectedElements();
	void HideUnselectedElements();

	//! hide selected faces
	void HideSelectedFaces();

	//! hide selected edges
	void HideSelectedEdges();

	//! hide selected nodes
	void HideSelectedNodes();

	// --- F A C E   D A T A ---
	void FaceNodePosition (FEFace& f, vec3f* r);
	void FaceNodeNormals  (FEFace& f, vec3f* n);
	void FaceNodeTexCoords(FEFace& f, float* t, bool bnode);

protected:
	void BuildFaces();
	void FindFaceNeighbors();
	void FindNeighbours();
	void AutoSmooth();
	void UpdateNodes();
	void BuildEdges();
	void UpdateDomains();

	void ClearDomains();
	void ClearParts();
	void ClearSurfaces();
	void ClearNodeSets();

protected:
	// --- A T T R I B U T E S ---
	double	m_stol;	// smoothing threshold

	// --- G E O M E T R Y ---
	vector<FENode>		m_Node;	// nodal array
	vector<FEEdge>		m_Edge;	// edge array
	vector<FEFace>		m_Face;	// face array
	vector<FEElement>	m_Elem;	// element array
	vector<FEDomain*>	m_Dom;	// domains

	// user-defined partitions
	vector<FEPart*>		m_Part;	// parts
	vector<FESurface*>	m_Surf;	// surfaces
	vector<FENodeSet*>	m_NSet;	// node sets

	FENodeElemList		m_NEL;
	FENodeFaceList		m_NFL;

	int		m_nID;
};

// find the element and the iso-parametric coordinates of a point inside the mesh
bool FindElementRef(FEMesh& m, const vec3f& x, int& nelem, double r[3]);

#endif // !defined(AFX_FEMESH_H__4E540300_07D8_4732_BB8D_6570BB162180__INCLUDED_)
