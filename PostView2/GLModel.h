#pragma once
#include "PostViewLib/GLObject.h"
#include "GLDataMap.h"
#include "PostViewLib/FEModel.h"
#include <PostViewLib/GDecoration.h>
#include <list>

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

	void Update(int ntime, float dt, bool breset);

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
	void RenderOutline(CGLContext& rc);
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
	void RenderFaces(FEModel* ps, CGLContext& rc);

	void RenderAllElements();	// used for element selection

	void AddDecoration(GDecoration* pd);
	void RemoveDecoration(GDecoration* pd);

protected:
	void RenderFace(FEFace& face, FEMesh* pm, int ndivs, bool bnode);
	void RenderFace(FEFace& face, FEMesh* pm, GLCOLOR c[4], int ndivs, bool bnode);
	void RenderElement(FEElement& el, FEMesh* pm);
	void RenderElementOutline(FEElement& el, FEMesh* pm);
	void RenderFaceOutline(FEFace& face, FEMesh* pm, int ndivs);
	void RenderMaterial(FEModel* ps, int m);
	void RenderTransparentMaterial(CGLContext& rc, FEModel* ps, int m);

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
	bool		m_bnorm;		//!< calculate normals or not
	bool		m_bsmooth;		//!< render smooth or not
	bool		m_boutline;		//!< render as outline
	bool		m_bghost;		//!< render the ghost (undeformed outline)
	bool		m_bShell2Hex;	//!< render shells as hexes
	int			m_nshellref;	//!< shell reference surface
	int			m_nDivs;		//!< nr of element subdivisions
	int			m_nrender;		//!< render mode
	GLCOLOR		m_line_col;		//!< line color
	GLCOLOR		m_node_col;		//!< color for rendering (unselected) nodes
	GLCOLOR		m_sel_col;		//!< selection color

protected:
	FEModel*	m_ps;

	CGLDisplacementMap*	m_pdis;
	CGLColorMap*		m_pcol;
	std::list<GDecoration*>	m_decor;
};
