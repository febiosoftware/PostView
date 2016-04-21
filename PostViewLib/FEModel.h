// FEModel.h: interface for the FEModel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FESCENE_H__A88C5C01_5318_4768_8424_1F59D461D94C__INCLUDED_)
#define AFX_FESCENE_H__A88C5C01_5318_4768_8424_1F59D461D94C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FEMesh.h"
#include "FEMaterial.h"
#include "FEState.h"
#include "FEDataManager.h"
#include "bbox.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
// Class that describes an FEModel. A model consists of a mesh (in the future
// there can be multiple meshes to support remeshing), a list of materials
// and a list of states. The states contain the data associated with the model
class FEModel  
{
public:
	// con-/destructor
	FEModel();
	virtual ~FEModel();

	// clear all model data
	void Clear();

	// get/set title of model
	void SetTitle(const char* sztitle);
	const char* GetTitle() { return m_szTitle; }

	// get the FE mesh for this model
	FEMesh* GetMesh() { return &m_mesh; }

	// get the current active state
	int currentTime() { return m_ntime; }

	//! get the data manager
	FEDataManager* GetDataManager() { return m_pDM; }

	// --- M A T E R I A L S ---

	// return number of materials
	int Materials() { return m_Mat.size();  }

	// get a particular material
	FEMaterial* GetMaterial(int i) { return &m_Mat[i]; }

	// clear all materials
	void ClearMaterials() { m_Mat.clear(); }

	// add a material to the model
	void AddMaterial(FEMaterial& mat);

	// --- S T A T E   M A N A G M E N T ---
	//! add a state to the mesh
	void AddState(FEState* pFEState) { m_State.push_back(pFEState); }

	//! Add a new state at time
	void AddState(float ftime);

	//! insert a state at a particular time
	void InsertState(FEState* ps, float f);

	//! remove a state from the mesh
	void DeleteState(int n);

	//! get the nr of states
	int GetStates() { return (int) m_State.size(); }

	//! retrieve pointer to a state
	FEState* GetState(int nstate) { return m_State[nstate]; }

	//! retrieve pointer to active state
	FEState* GetActiveState() { return m_State[m_ntime]; }

	//! Add a new data field
	void AddDataField(FEDataField* pd);

	//! add a new data field constrained to a set
	void AddDataField(FEDataField* pd, vector<int>& L);

	//! delete a data field
	void DeleteDataField(FEDataField* pd);

	//! Copy a data field
	void CopyDataField(FEDataField* pd);

public:
	//! get the bounding box
	BOUNDINGBOX GetBoundingBox() { return m_bbox; }

	//! Update the bounding box
	void UpdateBoundingBox();

	// Clear all states
	void ClearStates();

	// --- E V A L U A T I O N ---
	bool Evaluate(int nfield, int ntime, bool breset = false);

	// get the nodal coordinates of an element at time
	void GetElementCoords(int iel, int ntime, vec3f* r);

	// evaluate scalar functions
	void EvaluateNode   (int n, int ntime, int nfield, NODEDATA& d);
	void EvaluateFace   (int n, int ntime, int nfield, FACEDATA& d);
	void EvaluateElement(int n, int ntime, int nfield, ELEMDATA& d);

	// evaluate vector functions
	vec3f EvaluateNodeVector(int n, int ntime, int nvec);
	bool EvaluateFaceVector(int n, int ntime, int nvec, vec3f& r);
	vec3f EvaluateElemVector(int n, int ntime, int nvec);

	// evaluate tensor functions
	mat3fs EvaluateNodeTensor(int n, int ntime, int nten);
	mat3fs EvaluateFaceTensor(int n, int ntime, int nten);
	mat3fs EvaluateElemTensor(int n, int ntime, int nten);

	// displacement field
	void SetDisplacementField(int ndisp) { m_ndisp = ndisp; }
	int GetDisplacementField() { return m_ndisp; }
	vec3f NodePosition(int n, int ntime);
	vec3f FaceNormal(FEFace& f, int ntime);

	// checks if the field code is valid for the given state
	bool IsValidFieldCode(int nfield, int nstate);

public:
	static FEModel* GetInstance();

protected:
	// Helper functions for data evaluation
	void EvalNodeField(int ntime, int nfield);
	void EvalFaceField(int ntime, int nfield);
	void EvalElemField(int ntime, int nfield);
	
protected:
	char		m_szTitle[256]; // title of project
	int			m_ntime;		// time step that is being evaluated

	// --- M E S H ---
	FEMesh		m_mesh;	// the one and only mesh
	BOUNDINGBOX	m_bbox;		// bounding box of mesh

	// --- M A T E R I A L S ---
	vector<FEMaterial>	m_Mat;		// array of materials

	// --- S T A T E ---
	vector<FEState*>	m_State;	// array of pointers to FE-state structures
	FEDataManager*		m_pDM;		// the Data Manager
	int					m_ndisp;	// vector field defining the displacement

	static FEModel*	m_pThis;
};

#endif // !defined(AFX_FESCENE_H__A88C5C01_5318_4768_8424_1F59D461D94C__INCLUDED_)
