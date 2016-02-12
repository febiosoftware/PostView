#include "stdafx.h"
#include "FEState.h"
#include "FEMesh.h"
#include "FEModel.h"
#include "FEMeshData_T.h"

//-----------------------------------------------------------------------------
// Constructor
FEState::FEState(float time, FEModel* pfem)
{
	int i, N;

	FEMesh* pm = pfem->GetMesh();


	int nodes = pm->Nodes();
	int elems = pm->Elements();
	int faces = pm->Faces();

	// allocate storage
	m_NODE.resize(nodes);
	m_ELEM.resize(elems);
	m_FACE.resize(faces);

	// initialize data
	for (i=0; i<nodes; ++i) m_NODE[i].m_rt = pm->Node(i).m_r0;
	for (i=0; i<elems; ++i)
	{
		m_ELEM[i].m_h[0] = 0.f;
		m_ELEM[i].m_h[1] = 0.f;
		m_ELEM[i].m_h[2] = 0.f;
		m_ELEM[i].m_h[3] = 0.f;
	}

	m_time = time;
	m_nField = -1;

	// get the data manager
	FEDataManager* pdm = pfem->GetDataManager();

	// Nodal data
	N = pdm->NodeFields();
	FEDataFieldPtr pn = pdm->FirstNode();
	for (i=0; i<N; ++i, ++pn) m_Node.push_back((*pn)->CreateData(pfem));

	// Element data
	N = pdm->ElementFields();
	FEDataFieldPtr pe = pdm->FirstElement();
	for (i=0; i<N; ++i, ++pe) m_Elem.push_back((*pe)->CreateData(pfem));

	// Face data
	N = pdm->FaceFields();
	FEDataFieldPtr pf = pdm->FirstFace();
	for (i=0; i<N; ++i, ++pf) m_Face.push_back((*pf)->CreateData(pfem));
}

//-----------------------------------------------------------------------------
// Constructor
FEState::FEState(float time, FEModel* pfem, FEState* pstate)
{
	int i, N;

	FEMesh* pm = pfem->GetMesh();

	int nodes = pm->Nodes();
	int elems = pm->Elements();
	int faces = pm->Faces();

	// allocate storage
	m_NODE.resize(nodes);
	m_ELEM.resize(elems);
	m_FACE.resize(faces);

	// initialize data
	for (i=0; i<nodes; ++i) m_NODE[i].m_rt = pm->Node(i).m_r0;
	for (i=0; i<elems; ++i)
	{
		m_ELEM[i].m_h[0] = 0.f;
		m_ELEM[i].m_h[1] = 0.f;
		m_ELEM[i].m_h[2] = 0.f;
		m_ELEM[i].m_h[3] = 0.f;
	}

	m_time = time;
	m_nField = -1;

	// get the data manager
	FEDataManager* pdm = pfem->GetDataManager();

	// Nodal data
	N = pdm->NodeFields();
	FEDataFieldPtr pn = pdm->FirstNode();
	for (i=0; i<N; ++i, ++pn) m_Node.push_back((*pn)->CreateData(pfem));

	// copy nodal data
	N = m_Node.size();
	assert(m_Node.size() == pstate->m_Node.size());
	for (i=0; i<N; ++i)
	{
		FEMeshData& md = m_Node[i];
		switch (md.GetType())
		{
		case DATA_FLOAT: 
			{
				FENodeData<float>* pn = dynamic_cast<FENodeData<float>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<float>&>(pstate->m_Node[i]));
			}
			break;
		case DATA_VEC3F:
			{
				FENodeData<vec3f>* pn = dynamic_cast<FENodeData<vec3f>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<vec3f>&>(pstate->m_Node[i]));
			}
			break;
		case DATA_MAT3F:
			{
				FENodeData<mat3f>* pn = dynamic_cast<FENodeData<mat3f>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<mat3f>&>(pstate->m_Node[i]));
			}
			break;
		case DATA_MAT3D:
			{
				FENodeData<mat3d>* pn = dynamic_cast<FENodeData<mat3d>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<mat3d>&>(pstate->m_Node[i]));
			}
			break;
		case DATA_MAT3FS:
			{
				FENodeData<mat3fs>* pn = dynamic_cast<FENodeData<mat3fs>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<mat3fs>&>(pstate->m_Node[i]));
			}
			break;
		case DATA_MAT3FD:
			{
				FENodeData<mat3fd>* pn = dynamic_cast<FENodeData<mat3fd>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<mat3fd>&>(pstate->m_Node[i]));
			}
			break;
        case DATA_TENS4FS:
			{
				FENodeData<tens4fs>* pn = dynamic_cast<FENodeData<tens4fs>*>(&md);
				if (pn) pn->copy(dynamic_cast<FENodeData<tens4fs>&>(pstate->m_Node[i]));
			}
            break;
		}
	}

	// Face data
	N = pdm->FaceFields();
	FEDataFieldPtr pf = pdm->FirstFace();
	for (i=0; i<N; ++i, ++pf) m_Face.push_back((*pf)->CreateData(pfem));

	// copy face data
	N = m_Face.size();
	assert(m_Face.size() == pstate->m_Face.size());
	for (i=0; i<N; ++i)
	{
		FEMeshData& md = m_Face[i];
		switch (md.GetType())
		{
		case DATA_FLOAT: 
			{
				FEFaceData<float,DATA_ITEM>* pe = dynamic_cast<FEFaceData<float,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<float,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_VEC3F:
			{
				FEFaceData<vec3f,DATA_ITEM>* pe = dynamic_cast<FEFaceData<vec3f,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<vec3f,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3F:
			{
				FEFaceData<mat3f,DATA_ITEM>* pe = dynamic_cast<FEFaceData<mat3f,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<mat3f,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3D:
			{
				FEFaceData<mat3d,DATA_ITEM>* pe = dynamic_cast<FEFaceData<mat3d,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<mat3d,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3FS:
			{
				FEFaceData<mat3fs,DATA_ITEM>* pe = dynamic_cast<FEFaceData<mat3fs,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<mat3fs,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3FD:
			{
				FEFaceData<mat3fd,DATA_ITEM>* pe = dynamic_cast<FEFaceData<mat3fd,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<mat3fd,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
        case DATA_TENS4FS:
			{
				FEFaceData<tens4fs,DATA_ITEM>* pe = dynamic_cast<FEFaceData<tens4fs,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEFaceData<tens4fs,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
            break;
		}
	}

	// Element data
	N = pdm->ElementFields();
	FEDataFieldPtr pe = pdm->FirstElement();
	for (i=0; i<N; ++i, ++pe) m_Elem.push_back((*pe)->CreateData(pfem));

	// copy element data
	N = m_Elem.size();
	assert(m_Elem.size() == pstate->m_Elem.size());
	for (i=0; i<N; ++i)
	{
		FEMeshData& md = m_Elem[i];
		switch (md.GetType())
		{
		case DATA_FLOAT: 
			{
				FEElementData<float,DATA_ITEM>* pe = dynamic_cast<FEElementData<float,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<float,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_VEC3F:
			{
				FEElementData<vec3f,DATA_ITEM>* pe = dynamic_cast<FEElementData<vec3f,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<vec3f,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3F:
			{
				FEElementData<mat3f,DATA_ITEM>* pe = dynamic_cast<FEElementData<mat3f,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<mat3f,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3D:
			{
				FEElementData<mat3d,DATA_ITEM>* pe = dynamic_cast<FEElementData<mat3d,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<mat3d,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3FS:
			{
				FEElementData<mat3fs,DATA_ITEM>* pe = dynamic_cast<FEElementData<mat3fs,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<mat3fs,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
		case DATA_MAT3FD:
			{
				FEElementData<mat3fd,DATA_ITEM>* pe = dynamic_cast<FEElementData<mat3fd,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<mat3fd,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
			break;
        case DATA_TENS4FS:
			{
				FEElementData<tens4fs,DATA_ITEM>* pe = dynamic_cast<FEElementData<tens4fs,DATA_ITEM>*>(&md);
				if (pe) pe->copy(dynamic_cast<FEElementData<tens4fs,DATA_ITEM>&>(pstate->m_Elem[i]));
			}
            break;
		}
	}
}

//-----------------------------------------------------------------------------
void FEState::AddLine(vec3f a, vec3f b)
{
	LINEDATA L;
	L.m_r0 = a;
	L.m_r1 = b;
	m_Line.push_back(L);
}

//-----------------------------------------------------------------------------
void FEState::AddPoint(vec3f a, int nlabel)
{
	POINTDATA p;
	p.m_r = a;
	p.nlabel = nlabel;
	m_Point.push_back(p);
}
