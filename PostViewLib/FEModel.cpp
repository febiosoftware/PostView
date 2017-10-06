// FEModel.cpp: implementation of the FEModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEModel.h"
#include "FEDataManager.h"
#include "constants.h"
#include "FEMeshData_T.h"
#include <stdio.h>

extern int ET_HEX[12][2];

FEModel* FEModel::m_pThis = 0;

//=============================================================================
//								F E M O D E L
//=============================================================================
// constructor
FEModel::FEModel()
{
	m_szTitle[0] = 0;
	m_ndisp = 0;
	m_pDM = new FEDataManager(this);
	m_ntime = 0;

	m_pThis = this;
}

//-----------------------------------------------------------------------------
// desctructor
FEModel::~FEModel()
{
	Clear();
	delete m_pDM;
	m_pThis = 0;

	DeleteMeshes();

	ClearDependants();
}

void FEModel::DeleteMeshes()
{
	// delete all meshes
	for (size_t i = 0; i<m_mesh.size(); ++i) delete m_mesh[i];
	m_mesh.clear();
}


FEModel* FEModel::GetInstance()
{
	return m_pThis;
}

//-----------------------------------------------------------------------------
void FEModel::AddMesh(FEMeshBase* mesh)
{
	m_mesh.push_back(mesh);
}

//-----------------------------------------------------------------------------
int FEModel::Meshes() const
{
	return (int) m_mesh.size();
}

//-----------------------------------------------------------------------------
FEMeshBase* FEModel::GetFEMesh(int n)
{
	return m_mesh[n];
}

//-----------------------------------------------------------------------------
// Clear the data of the model
// TODO: This does not delete the mesh. Should I?
void FEModel::Clear()
{
	DeleteMeshes();

	m_Mat.clear();
	ClearStates();
	
	m_ntime = 0;
	m_szTitle[0] = 0;
}

//-----------------------------------------------------------------------------
void FEModel::SetTitle(const char* sztitle)
{
	strcpy(m_szTitle, sztitle); 
}

//-----------------------------------------------------------------------------
// add a material to the model
void FEModel::AddMaterial(FEMaterial& mat)
{ 
	static int n = 1;
	if (m_Mat.empty()) n = 1;

	if (mat.GetName()[0] == 0)
	{
		char sz[64];
		sprintf(sz, "Material%02d", n);
		n += 1;
		mat.SetName(sz);
	}
	m_Mat.push_back(mat); 
}

//-----------------------------------------------------------------------------
// clear the FE-states
void FEModel::ClearStates()
{
	for (int i=0; i<(int) m_State.size(); i++) delete m_State[i];
	m_State.clear();
}

//-----------------------------------------------------------------------------
void FEModel::AddState(FEState* pFEState)
{
	pFEState->SetID((int) m_State.size());
	m_State.push_back(pFEState); 
}

//-----------------------------------------------------------------------------
// add a state
void FEModel::AddState(float ftime)
{
	vector<FEState*>::iterator it = m_State.begin();
	for (it = m_State.begin(); it != m_State.end(); ++it)
		if ((*it)->m_time > ftime)
		{
			m_State.insert(it, new FEState(ftime, this, (*it)->GetFEMesh()));
			return;
		}

	// get last state
	FEState* ps = GetState(GetStates()-1);
	ps->SetID((int)m_State.size());
	m_State.push_back(new FEState(ftime, this, ps->GetFEMesh()));
}

//-----------------------------------------------------------------------------
// delete a state
void FEModel::DeleteState(int n)
{
	vector<FEState*>::iterator it = m_State.begin();
	int N = m_State.size();
	assert((n>=0) && (n<N));
	for (int i=0; i<n; ++i) ++it;
	m_State.erase(it);

	// reindex the states
	for (int i=0; i<(int)m_State.size(); ++i) m_State[i]->SetID(i);
}

//-----------------------------------------------------------------------------
// insert a state a time f
void FEModel::InsertState(FEState *ps, float f)
{
	vector<FEState*>::iterator it = m_State.begin();
	for (it=m_State.begin(); it != m_State.end(); ++it)
		if ((*it)->m_time > f) 
		{
			m_State.insert(it, ps);
			return;
		}
	m_State.push_back(ps);

	// reindex the states
	for (int i = 0; i<(int)m_State.size(); ++i) m_State[i]->SetID(i);
}

//-----------------------------------------------------------------------------
template <typename Type> void copy_node_data(FEMeshData& d, FEMeshData& s)
{
	FENodeData<Type>& dt = dynamic_cast<FENodeData<Type>&>(d);
	FENodeData<Type>& st = dynamic_cast<FENodeData<Type>&>(s);
	dt.copy(st);
}

//-----------------------------------------------------------------------------
template <typename Type, Data_Format Fmt> void copy_elem_data(FEMeshData& d, FEMeshData& s)
{
	FEElementData<Type, Fmt>& dt = dynamic_cast<FEElementData<Type, Fmt>&>(d);
	FEElementData<Type, Fmt>& st = dynamic_cast<FEElementData<Type, Fmt>&>(s);
	dt.copy(st);
}

//-----------------------------------------------------------------------------
template <typename Type, Data_Format Fmt> void copy_face_data(FEMeshData& d, FEMeshData& s)
{
	FEFaceData<Type, Fmt>& dt = dynamic_cast<FEFaceData<Type, Fmt>&>(d);
	FEFaceData<Type, Fmt>& st = dynamic_cast<FEFaceData<Type, Fmt>&>(s);
	dt.copy(st);
}

//-----------------------------------------------------------------------------
// Copy a data field
FEDataField* FEModel::CopyDataField(FEDataField* pd, const char* sznewname)
{
	// Clone the data field
	FEDataField* pdcopy = pd->Clone();

	// create a new name
	if (sznewname == 0)
	{
		char szname[256] = {0};
		sprintf(szname, "%s_copy", pd->GetName().c_str());
		pdcopy->SetName(szname);
	}
	else pdcopy->SetName(sznewname);

	// Add it to the model
	AddDataField(pdcopy);

	int ndst = FIELD_CODE(pdcopy->GetFieldID());
	int nsrc = FIELD_CODE(pd    ->GetFieldID());

	int nstates = GetStates();
	for (int i=0; i<nstates; ++i)
	{
		FEState& state = *GetState(i);
		FEMeshDataList& DL = state.m_Data;

		if (IS_NODE_FIELD(pd->GetFieldID()))
		{
			assert(pd->Format() == DATA_ITEM);
			if      (pd->Type() == DATA_FLOAT ) copy_node_data<float >(DL[ndst], DL[nsrc]);
			else if (pd->Type() == DATA_VEC3F ) copy_node_data<vec3f >(DL[ndst], DL[nsrc]);
			else if (pd->Type() == DATA_MAT3FS) copy_node_data<mat3fs>(DL[ndst], DL[nsrc]);
			else if (pd->Type() == DATA_MAT3FD) copy_node_data<mat3fd>(DL[ndst], DL[nsrc]);
			else if (pd->Type() == DATA_MAT3F ) copy_node_data<mat3f >(DL[ndst], DL[nsrc]);
			else if (pd->Type() == DATA_MAT3D ) copy_node_data<mat3d >(DL[ndst], DL[nsrc]);
		}
		else if (IS_FACE_FIELD(pd->GetFieldID()))
		{
			switch (pd->Format())
			{
			case DATA_ITEM:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_face_data<float , DATA_ITEM>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_face_data<vec3f , DATA_ITEM>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_face_data<mat3fs, DATA_ITEM>(DL[ndst], DL[nsrc]);
				}
				break;
			case DATA_NODE:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_face_data<float , DATA_NODE>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_face_data<vec3f , DATA_NODE>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_face_data<mat3fs, DATA_NODE>(DL[ndst], DL[nsrc]);
				}
				break;
			case DATA_COMP:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_face_data<float , DATA_COMP>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_face_data<vec3f , DATA_COMP>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_face_data<mat3fs, DATA_COMP>(DL[ndst], DL[nsrc]);
				}
				break;
			}
		}
		else if (IS_ELEM_FIELD(pd->GetFieldID()))
		{
			switch (pd->Format())
			{
			case DATA_ITEM:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_elem_data<float , DATA_ITEM>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_elem_data<vec3f , DATA_ITEM>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_elem_data<mat3fs, DATA_ITEM>(DL[ndst], DL[nsrc]);
				}
				break;
			case DATA_NODE:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_elem_data<float , DATA_NODE>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_elem_data<vec3f , DATA_NODE>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_elem_data<mat3fs, DATA_NODE>(DL[ndst], DL[nsrc]);
				}
				break;
			case DATA_COMP:
				{
					if      (pd->Type() == DATA_FLOAT ) copy_elem_data<float , DATA_COMP>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_VEC3F ) copy_elem_data<vec3f , DATA_COMP>(DL[ndst], DL[nsrc]);
					else if (pd->Type() == DATA_MAT3FS) copy_elem_data<mat3fs, DATA_COMP>(DL[ndst], DL[nsrc]);
				}
				break;
			}
		}
	}

	return pdcopy;
}

//-----------------------------------------------------------------------------
// Delete a data field
void FEModel::DeleteDataField(FEDataField* pd)
{
	// find out which data field this is
	FEDataFieldPtr it = m_pDM->FirstDataField();
	int NDF = m_pDM->DataFields(), m = -1;
	for (int i=0; i<NDF; ++i, ++it)
	{
		if (*it == pd)
		{
			m = i;
			break;
		}
	}
	if (m == -1) { assert(false); return; }

	// remove this field from all states
	int NS = GetStates();
	for (int i=0; i<NS; ++i)
	{
		FEState* ps = GetState(i);
		ps->m_Data.erase(m);
	}
	m_pDM->DeleteDataField(pd);

	// Inform all dependants
	UpdateDependants();
}

//-----------------------------------------------------------------------------
// Add a data field to all states of the model
void FEModel::AddDataField(FEDataField* pd)
{
	// add the data field to the data manager
	m_pDM->AddDataField(pd);

	// now add new data for each of the states
	vector<FEState*>::iterator it;
	for (it=m_State.begin(); it != m_State.end(); ++it)
	{
		(*it)->m_Data.push_back(pd->CreateData(*it));
	}

	// update all dependants
	UpdateDependants();
}

//-----------------------------------------------------------------------------
// Add an data field to all states of the model
void FEModel::AddDataField(FEDataField* pd, vector<int>& L)
{
	assert(pd->DataClass() == CLASS_FACE);

	// add the data field to the data manager
	m_pDM->AddDataField(pd);

	// now add new meshdata for each of the states
	vector<FEState*>::iterator it;
	for (it=m_State.begin(); it != m_State.end(); ++it)
	{
		FEFaceItemData* pmd = dynamic_cast<FEFaceItemData*>(pd->CreateData(*it));
		(*it)->m_Data.push_back(pmd);
		if (dynamic_cast<FECurvature*>(pmd))
		{
			FECurvature* pcrv = dynamic_cast<FECurvature*>(pmd);
			pcrv->set_facelist(L);
		}
		if (dynamic_cast<FECongruency*>(pmd))
		{
			FECongruency* pcon = dynamic_cast<FECongruency*>(pmd);
			pcon->set_facelist(L);
		}
	}

	// update all dependants
	UpdateDependants();
}

//-----------------------------------------------------------------------------
// This function calculates the position of a node based on the selected
// displacement field.
vec3f FEModel::NodePosition(int n, int ntime)
{
	FEMeshBase* mesh = GetState(ntime)->GetFEMesh();
	vec3f r = mesh->Node(n).m_r0;
	if (m_ndisp) r += EvaluateNodeVector(n, ntime, m_ndisp);
	
	return r;
}

//-----------------------------------------------------------------------------
vec3f FEModel::NodePosition(const vec3f& r, int ntime)
{
	FEMeshBase* mesh = GetState(ntime)->GetFEMesh();

	// find the element in which this node lies
	int iel = -1; double iso[3] = {0};
	if (FindElementInReferenceFrame(*mesh, r, iel, iso))
	{
		vec3f x[FEGenericElement::MAX_NODES];
		GetElementCoords(iel, ntime, x);

		// evaluate 
		FEElement& el = mesh->Element(iel);

		vec3f xt = el.eval(x, iso[0], iso[1], iso[2]);

		return xt;
	}
	else 
	{
		assert(false);
		return r;
	}
}

//-----------------------------------------------------------------------------
vec3f FEModel::FaceNormal(FEFace& f, int ntime)
{
	vec3f r0 = NodePosition(f.node[0], ntime);
	vec3f r1 = NodePosition(f.node[1], ntime);
	vec3f r2 = NodePosition(f.node[2], ntime);
	vec3f fn = (r1 - r0)^(r2 - r0);
	fn.Normalize();
	return fn;
}

//-----------------------------------------------------------------------------
// get the nodal coordinates of an element at time n
void FEModel::GetElementCoords(int iel, int ntime, vec3f* r)
{
	FEMeshBase* mesh = GetState(ntime)->GetFEMesh();
	FEElement& elem = mesh->Element(iel);
	NODEDATA* pn = &m_State[ntime]->m_NODE[0];

	for (int i=0; i<elem.Nodes(); i++)
		r[i] = pn[ elem.m_node[i] ].m_rt;
}

//-----------------------------------------------------------------------------
// Update the bounding box of the mesh. Note that this box bounds the reference
// configuration, not the current configuration
void FEModel::UpdateBoundingBox()
{
	FEMeshBase* mesh = GetFEMesh(0);
	FENode& n = mesh->Node(0);
	m_bbox.x0 = m_bbox.x1 = n.m_r0.x;
	m_bbox.y0 = m_bbox.y1 = n.m_r0.y;
	m_bbox.z0 = m_bbox.z1 = n.m_r0.z;

	int N = mesh->Nodes();
	for (int i=0; i<N; i++)
	{
		FENode& n = mesh->Node(i);
		if (n.m_r0.x < m_bbox.x0) m_bbox.x0 = n.m_r0.x;
		if (n.m_r0.y < m_bbox.y0) m_bbox.y0 = n.m_r0.y;
		if (n.m_r0.z < m_bbox.z0) m_bbox.z0 = n.m_r0.z;

		if (n.m_r0.x > m_bbox.x1) m_bbox.x1 = n.m_r0.x;
		if (n.m_r0.y > m_bbox.y1) m_bbox.y1 = n.m_r0.y;
		if (n.m_r0.z > m_bbox.z1) m_bbox.z1 = n.m_r0.z;
	}
}

//-----------------------------------------------------------------------------
void FEModel::AddDependant(FEModelDependant* pc)
{
	// make sure we have not added this dependant yet
	if (m_Dependants.empty() == false)
	{
		for (size_t i=0; i<m_Dependants.size(); ++i)
		{
			if (m_Dependants[i] == pc) return;
		}
	}

	// if we get here, the depedant was not added yet, so add it
	m_Dependants.push_back(pc);
}

//-----------------------------------------------------------------------------
void FEModel::UpdateDependants()
{
	int N = m_Dependants.size();
	for (int i=0; i<N; ++i) m_Dependants[i]->Update(this);
}

//-----------------------------------------------------------------------------
void FEModel::RemoveDependant(FEModelDependant* pc)
{
	int N = m_Dependants.size();
	if (N > 0)
	{
		vector<FEModelDependant*>::iterator it = m_Dependants.begin();
		for (int i=0; i<N; ++i, it++) 
		{
			if (m_Dependants[i] == pc) 
			{
				m_Dependants.erase(it);
				return;
			}
		}
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void FEModel::ClearDependants()
{
	int N = m_Dependants.size();
	if (N > 0)
	{
		// inform the dependents that the model is about to be deleted
		vector<FEModelDependant*>::iterator it = m_Dependants.begin();
		for (int i = 0; i<N; ++i) m_Dependants[i]->Update(0);
		m_Dependants.clear();
	}
}
