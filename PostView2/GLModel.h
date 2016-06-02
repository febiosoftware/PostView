#pragma once
#include "PostViewLib/GLObject.h"
#include "GLDataMap.h"
#include "PostViewLib/FEModel.h"
#include <PostViewLib/GDecoration.h>
#include <list>

// the selection modes
#define SELECT_NODES	1
#define SELECT_EDGES	2
#define SELECT_FACES	4
#define SELECT_ELEMS	8
#define SELECT_ADD		16
#define SELECT_SUB		32

class GLSurface
{
public:
	GLSurface(){}

	void add(const FEFace& f) { m_Face.push_back(f); }

	int Faces() const { return (int) m_Face.size(); }

	FEFace& Face(int i) { return m_Face[i]; }

private:
	vector<FEFace>	m_Face;
};

class CGLModel : public CGLVisual
{
public:
	CGLModel(FEModel* ps);
	~CGLModel(void);

	void Render(CGLContext& rc);
	CGLDisplacementMap* GetDisplacementMap() { return m_pdis; }
	CGLColorMap* GetColorMap() { return m_pcol; }
	FEMesh* GetMesh() { return m_ps->GetMesh(); }
	FEModel* GetFEModel() { return m_ps; }

	void Update(bool breset);

	bool AddDisplacementMap();

	void SetMaterialParams(FEMaterial* pm);

public:
	bool ShowNormals() { return m_bnorm; }
	void ShowNormals(bool b) { m_bnorm = b; }

	bool RenderSmooth() { return m_bsmooth; }
	void RenderSmooth(bool b) { m_bsmooth = b; }

	void ShowShell2Hex(bool b) { m_bShell2Hex = b; }
	bool ShowShell2Hex() { return m_bShell2Hex; }

	int ShellReferenceSurface() { return m_nshellref; }
	void ShellReferenceSurface(int n) { m_nshellref = n; }

	void SetSubDivisions(int ndivs) { m_nDivs = ndivs; }
	int GetSubDivisions();

	int GetRenderMode() { return m_nrender; }
	void SetRenderMode(int nmode) { m_nrender = nmode; }

public:
	void RenderFaces  (CGLContext& rc);
	void RenderOutline(CGLContext& rc, int nmat = -1);
	void RenderNormals(CGLContext& rc);
	void RenderGhost  (CGLContext& rc);

	void RenderSelection(CGLContext& rc);

	void RenderDecorations();

//	void RenderFeatureEdges(FEModel* ps);
	void RenderMeshLines(FEModel* ps);
	void RenderMeshLines(FEModel* ps, int nmat);
	void RenderThickShell(FEFace& face, FEMesh* pm);
	void RenderThickQuad(FEFace& face, FEMesh* pm);
	void RenderThickTri(FEFace& face, FEMesh* pm);
	void RenderThickShellOutline(FEFace& face, FEMesh* pm);
	void RenderShadows(FEModel* ps, vec3f lp, float inf);
	void RenderNodes(FEModel* ps, CGLContext& rc);
	void RenderEdges(FEModel* ps, CGLContext& rc);

	// more efficien rendering functions for selection
	void RenderAllElements();
	void RenderAllFaces();
	void RenderAllEdges();
	void RenderAllNodes();

	void AddDecoration(GDecoration* pd);
	void RemoveDecoration(GDecoration* pd);

protected:
	void RenderFace(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderFace(FEFace& face, FEMesh* pm, GLCOLOR c[4], int ndivs, bool bnode);
	void RenderFEFace(FEFace& el, FEMesh* pm);
	void RenderElementOutline(FEElement& el, FEMesh* pm);
	void RenderFaceOutline(FEFace& face, FEMesh* pm, int ndivs);
	void RenderSolidMaterial(FEModel* ps, int m);
	void RenderTransparentMaterial(CGLContext& rc, FEModel* ps, int m);
	void RenderFaceEdge(FEFace& face, int j, FEMesh* pm, int ndivs);

	void RenderSmoothQUAD4(FEFace& f, FEMesh* pm, int ndivs, bool bnode);
	void RenderSmoothQUAD4(vec3f r[4], vec3f n[4], float t[4], int ndivs);

	void RenderSmoothQUAD8(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderSmoothQUAD8(vec3f r[8], vec3f n[8], float t[8], int ndivs);
	void RenderSmoothQUAD9(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderSmoothQUAD9(vec3f r[9], vec3f n[9], float t[9], int ndivs);
	void RenderSmoothTRI6(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderSmoothTRI6(vec3f r[6], vec3f n[8], float t[6], int ndivs);
	void RenderSmoothTRI7(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderSmoothTRI7(vec3f r[7], vec3f n[7], float t[7], int ndivs);

	void RenderQUAD4(FEFace& f, bool bsmooth, bool bnode);
	void RenderQUAD8(FEFace& f, bool bsmooth, bool bnode);
	void RenderQUAD9(FEFace& f, bool bsmooth, bool bnode);
	void RenderTRI3 (FEFace& f, bool bsmooth, bool bnode);
	void RenderTRI6 (FEFace& f, bool bsmooth, bool bnode);
	void RenderTRI7 (FEFace& f, bool bsmooth, bool bnode);

public:
	float currentTime() const { return m_fTime; }
	int currentTimeIndex() const { return m_nTime; }

	void setCurrentTimeIndex(int n);

	// set the active state closest to time t
	void SetTimeValue(float ftime);

	// get the time value of state n
	float GetTimeValue(int ntime);

	// get the state closest to time t
	int GetClosestTime(double t);

	// get the currently active state
	FEState* currentState();

public: // Selection
	const vector<FENode*>&		GetNodeSelection   () const { return m_nodeSelection; }
	const vector<FEEdge*>&		GetEdgeSelection   () const { return m_edgeSelection; }
	const vector<FEFace*>&		GetFaceSelection   () const { return m_faceSelection; }
	const vector<FEElement*>&	GetElementSelection() const { return m_elemSelection; }
	void UpdateSelectionLists(int mode = -1);
	void ClearSelectionLists();

	void SelectNodes(vector<int>& items, bool bclear);
	void SelectEdges(vector<int>& items, bool bclear);
	void SelectFaces(vector<int>& items, bool bclear);
	void SelectElements(vector<int>& items, bool bclear);

	//! clear selection
	void ClearSelection();

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
	void HideMaterial(int nmat);

	//! show elements by material ID
	void ShowMaterial(int nmat);

	//! enable elements by material ID
	void EnableMaterial(int nmat);

	//! disable elements by material ID
	void DisableMaterial(int nmat);

	//! hide selected elements
	void HideSelectedElements();
	void HideUnselectedElements();

	//! hide selected faces
	void HideSelectedFaces();

	//! hide selected edges
	void HideSelectedEdges();

	//! hide selected nodes
	void HideSelectedNodes();

protected:
	void UpdateInternalSurfaces();
	void ClearInternalSurfaces();

public:
	bool		m_bnorm;		//!< calculate normals or not
	bool		m_bsmooth;		//!< render smooth or not
	bool		m_bghost;		//!< render the ghost (undeformed outline)
	bool		m_bShell2Hex;	//!< render shells as hexes
	int			m_nshellref;	//!< shell reference surface
	int			m_nDivs;		//!< nr of element subdivisions
	int			m_nrender;		//!< render mode
	GLCOLOR		m_line_col;		//!< line color
	GLCOLOR		m_node_col;		//!< color for rendering (unselected) nodes
	GLCOLOR		m_sel_col;		//!< selection color

	float	m_fTime;		// current time value
	int		m_nTime;		// active time step

protected:
	FEModel*			m_ps;
	vector<GLSurface*>	m_surf;

	CGLDisplacementMap*		m_pdis;
	CGLColorMap*			m_pcol;

	// selected items
	vector<FENode*>		m_nodeSelection;
	vector<FEEdge*>		m_edgeSelection;
	vector<FEFace*>		m_faceSelection;
	vector<FEElement*>	m_elemSelection;

	// TODO: move to document?
	std::list<GDecoration*>	m_decor;
};
