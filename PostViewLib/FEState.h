#pragma once
#include "FEMeshData.h"
#include "FEElement.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
// forward declaration of the mesh
class FEModel;

//-----------------------------------------------------------------------------
class ValArray
{
public:
	ValArray(){}

	int itemSize(int n) const { return m_index[n+1] - m_index[n]; }

	// append an item with n values
	void append(int n);

	float value(int item, int index) const { return m_data[m_index[item] + index]; }
	float& value(int item, int index) { return m_data[m_index[item] + index]; }

protected:
	vector<int>		m_index;
	vector<float>	m_data;
};

//-----------------------------------------------------------------------------
// Data classes for mesh items
struct NODEDATA
{
	vec3f	m_rt;	// nodal position determined by displacement map
	float	m_val;	// current nodal value
	int		m_ntag;	// active flag
};

struct EDGEDATA
{
	float	m_val;		// current value
	int		m_ntag;		// active flag
	float	m_nv[FEEdge::MAX_NODES]; // nodal values
};

struct ELEMDATA
{
	float	m_val;		// current element value
	int		m_ntag;		// active flag
	float	m_h[4];	// shell thickness (TODO: Can we move this to the face data?)
};

struct FACEDATA
{
	int		m_ntag;		// active flag
	float	m_val;		// current face value
};

struct LINEDATA
{
	vec3f	m_r0;
	vec3f	m_r1;
};

struct POINTDATA
{
	int		nlabel;
	vec3f	m_r;
};

//-----------------------------------------------------------------------------
// This class stores a state of a model. A state is defined by data for each
// of the field variables associated by the model. 
class FEState
{
public:
	FEState(float time, FEModel* pm);
	FEState(float time, FEModel* pm, FEState* ps);

	void AddLine(vec3f a, vec3f b);

	void AddPoint(vec3f a, int nlabel = 0);

	LINEDATA& Line(int n) { return m_Line[n]; }
	int Lines() { return (int) m_Line.size(); }

	POINTDATA& Point(int n) { return m_Point[n]; }
	int Points() { return (int) m_Point.size(); }

public:
	float	m_time;		// time value
	int		m_nField;	// the field whos values are contained in m_pval
	bool	m_bsmooth;

	vector<NODEDATA>	m_NODE;		// nodal data
	vector<EDGEDATA>	m_EDGE;		// edge data
	vector<FACEDATA>	m_FACE;		// face data
	vector<ELEMDATA>	m_ELEM;		// element data
	vector<LINEDATA>	m_Line;		// line data
	vector<POINTDATA>	m_Point;	// point data

	ValArray	m_ElemData;	// element data
	ValArray	m_FaceData;	// face data

	// Data
	FEMeshDataList	m_Data;	// data
};
