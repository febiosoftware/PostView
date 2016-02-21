// GLPlaneCutPlot.h: interface for the CGLPlaneCutPlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLPLANECUTPLOT_H__54EFA62A_204A_4739_A28C_25E120219FE9__INCLUDED_)
#define AFX_GLPLANECUTPLOT_H__54EFA62A_204A_4739_A28C_25E120219FE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLPlot.h"
#include <vector>
using namespace std;

class CGLPlaneCutPlot : public CGLPlot  
{
public:
	CGLPlaneCutPlot(CGLModel* po);
	virtual ~CGLPlaneCutPlot();

	void SetBoundingBox(BOUNDINGBOX box) { m_box = box; }

	void SetRotation(float rot) { m_rot = rot; }
	float GetRotation() { return m_rot; }

	void GetPlaneEqn(GLdouble* eqn)
	{
		eqn[0] = m_eq[0];
		eqn[1] = m_eq[1];
		eqn[2] = m_eq[2];
		eqn[3] = m_eq[3];
	}

	void SetPlaneEqn(GLdouble a[4])
	{
		m_eq[0] = a[0];
		m_eq[1] = a[1];
		m_eq[2] = a[2];
		m_eq[3] = a[3];
	}

	void GetNormalizedEquations(double a[4]);
	vec3f GetPlaneNormal();
	float GetPlaneOffset();

	void Render(CGLContext& rc);
	void RenderPlane();
	float Integrate(FEState* ps);

	static void InitClipPlanes();
	static void DisableClipPlanes();
	static void EnableClipPlanes();

	void Activate(bool bact);

	CPropertyList* propertyList();

protected:
	void RenderSlice();
	void RenderMesh();
	void RenderOutline();
	vec3f WorldToPlane(vec3f r);

	void ReleasePlane();
	static int GetFreePlane();

public:
	bool	m_bshowplane;	// show the plane or not
	bool	m_bcut_hidden;	// cut hidden materials
	bool	m_bshow_mesh;

protected:
	GLdouble	m_eq[4];	// plane equation

	float		m_ref;	// reference = m_pos + m_off

	float		m_rot;	// rotation around z-axis
	BOUNDINGBOX	m_box;	// bounding box to cut

	struct EDGE
	{
		vec3f	m_r[2];	// position of nodes
		int		m_n[2];	// node numbers
		int		m_ntag;
	};

	int		m_nclip;			// clip plane number
	static	vector<int>	m_clip;	// avaialbe clip planes
};

#endif // !defined(AFX_GLPLANECUTPLOT_H__54EFA62A_204A_4739_A28C_25E120219FE9__INCLUDED_)
