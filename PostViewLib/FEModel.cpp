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
	// TODO: Set the default to 0
	m_ndisp = BUILD_FIELD(1,0,0);
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
}

FEModel* FEModel::GetInstance()
{
	return m_pThis;
}

//-----------------------------------------------------------------------------
// Clear the data of the model
void FEModel::Clear()
{
	m_mesh.CleanUp();
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
// add a state
void FEModel::AddState(float ftime)
{
	InsertState(new FEState(ftime, this), ftime);
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
}

//-----------------------------------------------------------------------------
// NOTE: This will not work since d,s can point to node data and face data as well
template <typename Type, Data_Format Fmt> void copy_data(FEMeshData& d, FEMeshData& s)
{
	FEElementData<Type, Fmt>& dt = dynamic_cast<FEElementData<Type, Fmt>&>(d);
	FEElementData<Type, Fmt>& st = dynamic_cast<FEElementData<Type, Fmt>&>(s);
	dt.copy(st);
}

//-----------------------------------------------------------------------------
// Copy a data field
void FEModel::CopyDataField(FEDataField* pd)
{
	char szname[256] = {0};
	sprintf(szname, "%s_copy", pd->GetName());
	FEDataField* pdcopy = pd->Clone();
	pdcopy->SetName(szname);
	AddDataField(pdcopy);

	int ndst = FIELD_CODE(pdcopy->GetFieldID());
	int nsrc = FIELD_CODE(pd    ->GetFieldID());

	int nstates = GetStates();
	for (int i=0; i<nstates; ++i)
	{
		FEState& state = *GetState(i);
		FEMeshDataList& DL = state.m_Data;

		switch (pd->Format())
		{
		case DATA_ITEM:
			{
				if      (pd->Type() == DATA_FLOAT ) copy_data<float , DATA_ITEM>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_VEC3F ) copy_data<vec3f , DATA_ITEM>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_MAT3FS) copy_data<mat3fs, DATA_ITEM>(DL[ndst], DL[nsrc]);
			}
			break;
		case DATA_NODE:
			{
				if      (pd->Type() == DATA_FLOAT ) copy_data<float , DATA_NODE>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_VEC3F ) copy_data<vec3f , DATA_NODE>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_MAT3FS) copy_data<mat3fs, DATA_NODE>(DL[ndst], DL[nsrc]);
			}
			break;
		case DATA_COMP:
			{
				if      (pd->Type() == DATA_FLOAT ) copy_data<float , DATA_COMP>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_VEC3F ) copy_data<vec3f , DATA_COMP>(DL[ndst], DL[nsrc]);
				else if (pd->Type() == DATA_MAT3FS) copy_data<mat3fs, DATA_COMP>(DL[ndst], DL[nsrc]);
			}
			break;
		}
	}
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
		(*it)->m_Data.push_back(pd->CreateData(this));
	}

	// create a new ID for the mesh so that all dependant
	// objects will update themselves
	m_mesh.NewID();
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
		FEFaceItemData* pmd = dynamic_cast<FEFaceItemData*>(pd->CreateData(this));
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

	// create a new ID for the mesh so that all dependant
	// objects will update themselves
	m_mesh.NewID();
}

//-----------------------------------------------------------------------------
// This function calculates the position of a node based on the selected
// displacement field.
vec3f FEModel::NodePosition(int n, int ntime)
{
	vec3f r = m_mesh.Node(n).m_r0;
	if (m_ndisp) r += EvaluateNodeVector(n, ntime, m_ndisp);
	
	return r;
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
	FEElement& elem = m_mesh.Element(iel);
	NODEDATA* pn = &m_State[ntime]->m_NODE[0];

	for (int i=0; i<8; i++)
		r[i] = pn[ elem.m_node[i] ].m_rt;
}

//-----------------------------------------------------------------------------
// Update the bounding box of the mesh. Note that this box bounds the reference
// configuration, not the current configuration
void FEModel::UpdateBoundingBox()
{
	FENode& n = m_mesh.Node(0);
	m_bbox.x0 = m_bbox.x1 = n.m_r0.x;
	m_bbox.y0 = m_bbox.y1 = n.m_r0.y;
	m_bbox.z0 = m_bbox.z1 = n.m_r0.z;

	int N = m_mesh.Nodes();
	for (int i=0; i<N; i++)
	{
		FENode& n = m_mesh.Node(i);
		if (n.m_r0.x < m_bbox.x0) m_bbox.x0 = n.m_r0.x;
		if (n.m_r0.y < m_bbox.y0) m_bbox.y0 = n.m_r0.y;
		if (n.m_r0.z < m_bbox.z0) m_bbox.z0 = n.m_r0.z;

		if (n.m_r0.x > m_bbox.x1) m_bbox.x1 = n.m_r0.x;
		if (n.m_r0.y > m_bbox.y1) m_bbox.y1 = n.m_r0.y;
		if (n.m_r0.z > m_bbox.z1) m_bbox.z1 = n.m_r0.z;
	}
}
