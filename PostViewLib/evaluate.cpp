#include "FEModel.h"
#include "constants.h"
#include "FEMeshData_T.h"

extern int FT_HEX[6][4];
extern int FT_TET[4][4];
extern int FT_PENTA[5][4];
extern int FT_HEX20[6][8];
extern int FT_HEX27[6][9];
extern int FT_TET10[4][6];
extern int FT_TET15[4][7];

//-----------------------------------------------------------------------------
// extract a component from a vector
float component(const vec3f& v, int n)
{
	float g = 0.f;
	switch(n)
	{
	case 0: g = v.x; break;
	case 1: g = v.y; break;
	case 2: g = v.z; break;
	case 3: g = sqrt(v.x*v.x + v.y*v.y); break;
	case 4: g = sqrt(v.y*v.y + v.z*v.z); break;
	case 5: g = sqrt(v.x*v.x + v.z*v.z); break;
	case 6: g = sqrt(v.x*v.x + v.y*v.y + v.z*v.z); break;
	}
	return g;
}

//-----------------------------------------------------------------------------
// extract a component from a mat3d
float component(const mat3d& m, int n)
{
	float g = 0.f;
	switch (n)
	{
	case 0: g = (float) m(0,0); break;
	case 1: g = (float) m(0,1); break;
	case 2: g = (float) m(0,2); break;
	case 3: g = (float) m(1,0); break;
	case 4: g = (float) m(1,1); break;
	case 5: g = (float) m(1,2); break;
	case 6: g = (float) m(2,0); break;
	case 7: g = (float) m(2,1); break;
	case 8: g = (float) m(2,2); break;
	default:
		assert(false);
	}
	return g;
}

//-----------------------------------------------------------------------------
// extract a component from a mat3d
float component(const mat3f& m, int n)
{
	float g = 0.f;
	switch (n)
	{
	case 0: g = m(0,0); break;
	case 1: g = m(0,1); break;
	case 2: g = m(0,2); break;
	case 3: g = m(1,0); break;
	case 4: g = m(1,1); break;
	case 5: g = m(1,2); break;
	case 6: g = m(2,0); break;
	case 7: g = m(2,1); break;
	case 8: g = m(2,2); break;
	default:
		assert(false);
	}
	return g;
}

//-----------------------------------------------------------------------------
// extract a component from a tensor
float component(const mat3fs& m, int n)
{
	float g = 0.f;
	switch (n)
	{
	case 0: g = m.x; break;
	case 1: g = m.y; break;
	case 2: g = m.z; break;
	case 3: g = m.xy; break;
	case 4: g = m.yz; break;
	case 5: g = m.xz; break;
	case 6: g = m.von_mises(); break;
	case 7: { float p[3]; m.Principals(p); g = p[0]; } break;
	case 8: { float p[3]; m.Principals(p); g = p[1]; } break;
	case 9: { float p[3]; m.Principals(p); g = p[2]; } break;
	case 10: { float p[3]; m.DeviatoricPrincipals(p); g = p[0]; } break;
	case 11: { float p[3]; m.DeviatoricPrincipals(p); g = p[1]; } break;
	case 12: { float p[3]; m.DeviatoricPrincipals(p); g = p[2]; } break;
	case 13: g = m.MaxShear(); break;
	default:
		assert(false);
	}
	return g;
}

//-----------------------------------------------------------------------------
// extract a component from a tensor
float component(const mat3fd& m, int n)
{
	float g = 0.f;
	switch (n)
	{
	case 0: g = m.x; break;
	case 1: g = m.y; break;
	case 2: g = m.z; break;
	default:
		assert(false);
	}
	return g;
}

//-----------------------------------------------------------------------------
// extract a component from a fourth-order tensor
float component(const tens4fs& m, int n)
{
	float g = 0.f;
    assert((n >= 0) && (n<21));
    g = m.d[n];
	return g;
}

//-----------------------------------------------------------------------------
bool FEModel::IsValidFieldCode(int nfield, int nstate)
{
	// check the state number first
	if ((nstate < 0) || (nstate >= GetStates())) return false;

	// get the state info
	FEState& state = *m_State[nstate];

	// get the data field
	int ndata = FIELD_CODE(nfield);

	// check the field sizes
	if (ndata < 0) return false;
	if (ndata >= state.m_Data.size()) return false;

	// if we get here, everything looks good
	return true;
}

//-----------------------------------------------------------------------------
// Evaluate a data field at a particular time
bool FEModel::Evaluate(int nfield, int ntime, bool breset)
{
	if (m_mesh.Nodes() == 0) return false;

	// get the state data 
	FEState& state = *m_State[ntime];

	// make sure that we have to reevaluate
	if ((state.m_nField != nfield) || breset)
	{
		// store the field variable
		state.m_nField = nfield;

		if      (IS_NODE_FIELD(nfield)) EvalNodeField(ntime, nfield);
		else if (IS_ELEM_FIELD(nfield)) EvalElemField(ntime, nfield);
		else if (IS_FACE_FIELD(nfield)) EvalFaceField(ntime, nfield);
		else assert(false);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Evaluate a nodal field
void FEModel::EvalNodeField(int ntime, int nfield)
{
	m_ntime = ntime;

	assert(IS_NODE_FIELD(nfield));

	// get the state data 
	FEState& state = *m_State[ntime];

	// first, we evaluate all the nodes
	int i, j;
	for (i=0; i<m_mesh.Nodes(); ++i)
	{
		FENode& node = m_mesh.Node(i);
		NODEDATA& d = state.m_NODE[i];
		d.m_val = 0;
		d.m_ntag = 0;
		if (node.IsEnabled()) EvaluateNode(i, ntime, nfield, d);
	}

	// Next, we project the nodal data onto the faces
	for (i=0; i<m_mesh.Faces(); ++i)
	{
		FEFace& f = m_mesh.Face(i);
		FACEDATA& d = state.m_FACE[i];
		d.m_val = 0.f;
		d.m_ntag = 0;
		if (f.IsEnabled())
		{
			d.m_ntag = 1;
			for (j=0; j<f.Nodes(); ++j) { d.m_nv[j] = state.m_NODE[f.node[j]].m_val; d.m_val += d.m_nv[j]; }
			d.m_val /= (float) f.Nodes();
		}
	}

	// Finally, we project the nodal data onto the elements
	for (i=0; i<m_mesh.Elements(); ++i)
	{
		FEElement& e = m_mesh.Element(i);
		ELEMDATA& d = state.m_ELEM[i];
		d.m_val = 0.f;
		d.m_ntag = 0;
		if (e.IsEnabled())
		{
			d.m_ntag = 1;
			for (j=0; j<e.Nodes(); ++j) { d.m_nv[j] = state.m_NODE[e.m_node[j]].m_val; d.m_val += d.m_nv[j]; }
			d.m_val /= (float) e.Nodes();
		}
	}
}

//-----------------------------------------------------------------------------
// Evaluate a face field variable
void FEModel::EvalFaceField(int ntime, int nfield)
{
	m_ntime = ntime;

	assert(IS_FACE_FIELD(nfield));

	// get the state data 
	FEState& state = *m_State[ntime];

	// first evaluate all faces
	int i, j;
	for (i=0; i<m_mesh.Faces(); ++i)
	{
		FEFace& f = m_mesh.Face(i);
		state.m_FACE[i].m_val = 0.f;
		state.m_FACE[i].m_ntag = 0;
		if (f.IsEnabled()) EvaluateFace(i, ntime, nfield, state.m_FACE[i]);
	}

	// now evaluate the nodes
	for (i=0; i<m_mesh.Nodes(); ++i)
	{
		NODEDATA& node = state.m_NODE[i];
		vector<NodeFaceRef>& nfl = m_mesh.NodeFaceList(i);
		node.m_val = 0.f; 
		node.m_ntag = 0;
		int n = 0;
		for (j=0; j<(int) nfl.size(); ++j)
		{
			FACEDATA& f = state.m_FACE[nfl[j].first];
			if (f.m_ntag > 0)
			{
				node.m_val += f.m_nv[nfl[j].second];
				++n;
			}
		}
		if (n > 0)
		{
			node.m_val /= (float) n;
			node.m_ntag = 1;
		}
	}

	// evaluate the elements (to zero)
	// Face data is not projected onto the elements
	for (i=0; i<m_mesh.Elements(); ++i) 
	{
		state.m_ELEM[i].m_val = 0.f;
		state.m_ELEM[i].m_ntag = 0;
	}
}

//-----------------------------------------------------------------------------
// Evaluate an Element field
void FEModel::EvalElemField(int ntime, int nfield)
{
	m_ntime = ntime;

	assert(IS_ELEM_FIELD(nfield));

	// get the state data 
	FEState& state = *m_State[ntime];

	// first evaluate all elements
	int i, j;
	for (i=0; i<m_mesh.Elements(); ++i)
	{
		FEElement& el = m_mesh.Element(i);
		state.m_ELEM[i].m_val = 0.f;
		state.m_ELEM[i].m_ntag = 0;
		if (el.IsEnabled()) EvaluateElement(i, ntime, nfield, state.m_ELEM[i]);
	}

	// now evaluate the nodes
	for (i=0; i<m_mesh.Nodes(); ++i)
	{
		FENode& node = m_mesh.Node(i);
		state.m_NODE[i].m_val = 0.f;
		state.m_NODE[i].m_ntag = 0;
		if (node.IsEnabled())
		{
			vector<NodeElemRef>& nel = m_mesh.NodeElemList(i);
			int m = (int) nel.size(), n=0;
			float val = 0.f;
			for (j=0; j<m; ++j)
			{
				ELEMDATA& e = state.m_ELEM[nel[j].first];
				if (e.m_ntag > 0)
				{
					val += e.m_nv[nel[j].second];
					++n;
				}
			}
			if (n != 0) 
			{
				state.m_NODE[i].m_val = val / (float) n;
				state.m_NODE[i].m_ntag = 1;
			}
		}
	}

	// evaluate faces
	for (i=0; i<m_mesh.Faces(); ++i)
	{
		FEFace& f = m_mesh.Face(i);
		FACEDATA& d = state.m_FACE[i];
		d.m_ntag = 0;
		if (state.m_ELEM[f.m_elem[0]].m_ntag > 0)
		{
			d.m_ntag = 1;
			ELEMDATA& e = state.m_ELEM[f.m_elem[0]];
			switch (m_mesh.Element(f.m_elem[0]).m_ntype)
			{
			case FE_TET4:
				{
					const int* fn = FT_TET[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2]) / 3.f;
				}
				break;
			case FE_PENTA6:
				{
					const int* fn = FT_PENTA[f.m_elem[1]];
                    switch (f.m_ntype) {
                        case FACE_TRI3:
                            d.m_nv[0] = e.m_nv[fn[0]];
                            d.m_nv[1] = e.m_nv[fn[1]];
                            d.m_nv[2] = e.m_nv[fn[2]];
                            d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2]) / 3.f;
                            break;
                        case FACE_QUAD4:
                            d.m_nv[0] = e.m_nv[fn[0]];
                            d.m_nv[1] = e.m_nv[fn[1]];
                            d.m_nv[2] = e.m_nv[fn[2]];
                            d.m_nv[3] = e.m_nv[fn[3]];
                            d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3]) / 4.f;
                            break;
                        default:
                            break;
                    }
				}
				break;
			case FE_HEX8:
				{
					const int* fn = FT_HEX[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_nv[3] = e.m_nv[fn[3]];
					d.m_val = 0.25f*(d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3]);
				}
				break;
			case FE_TET10:
				{
					const int* fn = FT_TET10[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_nv[3] = e.m_nv[fn[3]];
					d.m_nv[4] = e.m_nv[fn[4]];
					d.m_nv[5] = e.m_nv[fn[5]];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5]) / 6.f;
				}
				break;
			case FE_TET15:
				{
					const int* fn = FT_TET15[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_nv[3] = e.m_nv[fn[3]];
					d.m_nv[4] = e.m_nv[fn[4]];
					d.m_nv[5] = e.m_nv[fn[5]];
					d.m_nv[6] = e.m_nv[fn[6]];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6]) / 7.f;
				}
				break;
			case FE_HEX20:
				{
					const int* fn = FT_HEX20[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_nv[3] = e.m_nv[fn[3]];
					d.m_nv[4] = e.m_nv[fn[4]];
					d.m_nv[5] = e.m_nv[fn[5]];
					d.m_nv[6] = e.m_nv[fn[6]];
					d.m_nv[7] = e.m_nv[fn[7]];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])*0.125f;
				}
				break;
			case FE_HEX27:
				{
					const int* fn = FT_HEX27[f.m_elem[1]];
					d.m_nv[0] = e.m_nv[fn[0]];
					d.m_nv[1] = e.m_nv[fn[1]];
					d.m_nv[2] = e.m_nv[fn[2]];
					d.m_nv[3] = e.m_nv[fn[3]];
					d.m_nv[4] = e.m_nv[fn[4]];
					d.m_nv[5] = e.m_nv[fn[5]];
					d.m_nv[6] = e.m_nv[fn[6]];
					d.m_nv[7] = e.m_nv[fn[7]];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])*0.125f;
				}
				break;
			case FE_TRI3:
				{
					d.m_nv[0] = e.m_nv[0];
					d.m_nv[1] = e.m_nv[1];
					d.m_nv[2] = e.m_nv[2];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2])/3.0f;
				}
				break;
			case FE_QUAD4:
				{
					d.m_nv[0] = e.m_nv[0];
					d.m_nv[1] = e.m_nv[1];
					d.m_nv[2] = e.m_nv[2];
					d.m_nv[3] = e.m_nv[3];
					d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3])*0.25f;
				}
				break;
            case FE_QUAD8:
                {
                    d.m_nv[0] = e.m_nv[0];
                    d.m_nv[1] = e.m_nv[1];
                    d.m_nv[2] = e.m_nv[2];
                    d.m_nv[3] = e.m_nv[3];
                    d.m_nv[4] = e.m_nv[4];
                    d.m_nv[5] = e.m_nv[5];
                    d.m_nv[6] = e.m_nv[6];
                    d.m_nv[7] = e.m_nv[7];
                    d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])*0.125f;
                }
                break;
            case FE_QUAD9:
                {
                    d.m_nv[0] = e.m_nv[0];
                    d.m_nv[1] = e.m_nv[1];
                    d.m_nv[2] = e.m_nv[2];
                    d.m_nv[3] = e.m_nv[3];
                    d.m_nv[4] = e.m_nv[4];
                    d.m_nv[5] = e.m_nv[5];
                    d.m_nv[6] = e.m_nv[6];
                    d.m_nv[7] = e.m_nv[7];
                    d.m_nv[8] = e.m_nv[8];
                    d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])/9.0f;
                }
                break;
            case FE_TRI6:
                {
                    d.m_nv[0] = e.m_nv[0];
                    d.m_nv[1] = e.m_nv[1];
                    d.m_nv[2] = e.m_nv[2];
                    d.m_nv[3] = e.m_nv[3];
                    d.m_nv[4] = e.m_nv[4];
                    d.m_nv[5] = e.m_nv[5];
                    d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5])/6.0f;
                }
                    break;
			default:
				assert(false);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Get the field value of node n at time ntime
void FEModel::EvaluateNode(int n, int ntime, int nfield, NODEDATA& d)
{
	m_ntime = ntime;

	// the return value
	d.m_val = 0.f;
	d.m_ntag = 1;

	// get the state
	FEState& s = *m_State[ntime];

	if (IS_NODE_FIELD(nfield))
	{
		// get the data ID
		int ndata = FIELD_CODE(nfield);
		assert((ndata >= 0) && (ndata < s.m_Data.size()));

		// get the component
		int ncomp = FIELD_COMP(nfield);

		FEMeshData& rd = s.m_Data[ndata];
		Data_Format fmt = rd.GetFormat();
		assert(fmt == DATA_ITEM);

		switch (rd.GetType())
		{
		case DATA_FLOAT: 
			{
				FENodeData_T<float,DATA_ITEM>& df = dynamic_cast<FENodeData_T<float,DATA_ITEM>&>(rd);
				df.eval(n, &d.m_val);
			}
			break;
		case DATA_VEC3F:
			{
				FENodeData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FENodeData_T<vec3f,DATA_ITEM>&>(rd);
				vec3f v;
				dv.eval(n,&v);
				d.m_val = component(v, ncomp);
			}
			break;
		case DATA_MAT3F:
			{
				FENodeData_T<mat3f,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3f,DATA_ITEM>&>(rd);
				mat3f m;
				dm.eval(n, &m);
				d.m_val = component(m, ncomp);
			}
			break;
		case DATA_MAT3D:
			{
				FENodeData_T<mat3d,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3d,DATA_ITEM>&>(rd);
				mat3d m;
				dm.eval(n, &m);
				d.m_val = component(m, ncomp);
			}
			break;
		case DATA_MAT3FS:
			{
				FENodeData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3fs,DATA_ITEM>&>(rd);
				mat3fs m;
				dm.eval(n, &m);
				d.m_val = component(m, ncomp);
			}
			break;
		case DATA_MAT3FD:
			{
				FENodeData_T<mat3fd,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3fd,DATA_ITEM>&>(rd);
				mat3fd m;
				dm.eval(n, &m);
				d.m_val = component(m, ncomp);
			}
			break;
        case DATA_TENS4FS:
			{
				FENodeData_T<tens4fs,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<tens4fs,DATA_ITEM>&>(rd);
				tens4fs m;
				dm.eval(n, &m);
				d.m_val = component(m, ncomp);
			}
            break;
		}
	}
	else if (IS_FACE_FIELD(nfield))
	{
		// we take the average of the adjacent face values
		vector<NodeFaceRef>& nfl = m_mesh.NodeFaceList(n);
		if (!nfl.empty())
		{
			int nf = nfl.size(), n=0;
			FACEDATA f;
			for (int i=0; i<nf; ++i)
			{
				EvaluateFace(nfl[i].first, ntime, nfield, f);
				if (f.m_ntag > 0)
				{
					d.m_val += f.m_nv[nfl[i].second];
					++n;
				}
			}
			if (n>0) d.m_val /= (float) n;
		}
	}
	else if (IS_ELEM_FIELD(nfield))
	{
		// we take the average of the elements that contain this element
		vector<NodeElemRef>& nel = m_mesh.NodeElemList(n);
		ELEMDATA e;
		int ne = nel.size(),n=0;
		if (!nel.empty())
		{
			for (int i=0; i<ne; ++i)
			{
				EvaluateElement(nel[i].first, ntime, nfield, e);
				if (e.m_ntag > 0)
				{
					d.m_val += e.m_nv[nel[i].second];
					++n;
				}
			}
			d.m_val /= (float) n;
		}
	}
	else assert(false);
}

//-----------------------------------------------------------------------------
// Calculate field value of edge n at time ntime
void FEModel::EvaluateEdge(int n, int ntime, int nfield, EDGEDATA& d)
{
	d.m_val = 0.f;
	d.m_ntag = 0;
	d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = 0.f;
}

//-----------------------------------------------------------------------------
// Calculate field value of face n at time ntime
void FEModel::EvaluateFace(int n, int ntime, int nfield, FACEDATA& d)
{
	m_ntime = ntime;

	// the return value
	d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = 0;
	d.m_val = 0;
	d.m_ntag = 0;

	// get the state
	FEState& s = *m_State[ntime];

	// get the face
	FEFace& f = m_mesh.Face(n);

	if (IS_FACE_FIELD(nfield))
	{
		// get the data ID
		int ndata = FIELD_CODE(nfield);
		assert ((ndata >= 0) && (ndata < s.m_Data.size()));

		// get the component
		int ncomp = FIELD_COMP(nfield);

		FEMeshData& rd = s.m_Data[ndata];
		Data_Format fmt = rd.GetFormat();

		switch (rd.GetType())
		{
		case DATA_FLOAT: 
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<float,DATA_NODE>& df = dynamic_cast<FEFaceData_T<float,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						df.eval(n, d.m_nv);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<float,DATA_ITEM>& df = dynamic_cast<FEFaceData_T<float,DATA_ITEM>&>(rd);
					if (df.active(n))
					{
						df.eval(n, &d.m_val);
						int nf = f.Nodes();
						for (int i=0; i<nf; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<float,DATA_COMP>& df = dynamic_cast<FEFaceData_T<float,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						df.eval(n, d.m_nv);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<float,DATA_REGION>& df = dynamic_cast<FEFaceData_T<float,DATA_REGION>&>(rd);
					if (df.active(n))
					{
						df.eval(n, &d.m_val);
						int nf = f.Nodes();
						for (int i=0; i<nf; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
			}
			break;
		case DATA_VEC3F:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<vec3f,DATA_NODE>& df = dynamic_cast<FEFaceData_T<vec3f,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						vec3f v[FEFace::MAX_NODES];
						df.eval(n, v);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i)
						{
							d.m_nv[i] = component(v[i], ncomp);
							d.m_val += d.m_nv[i];
						}
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<vec3f,DATA_ITEM>&>(rd);
					vec3f v;
					dv.eval(n, &v);
					d.m_val = component(v, ncomp);
					int nf = f.Nodes();
					for (int i=0; i<nf; ++i) d.m_nv[i] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<vec3f,DATA_COMP>& df = dynamic_cast<FEFaceData_T<vec3f,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						vec3f v[FEFace::MAX_NODES];
						df.eval(n, v);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) 
						{
							d.m_nv[i] = component(v[i], ncomp);
							d.m_val += d.m_nv[i];
						}
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<vec3f,DATA_REGION>& dv = dynamic_cast<FEFaceData_T<vec3f,DATA_REGION>&>(rd);
					if (dv.active(n))
					{
						vec3f v;
						dv.eval(n, &v);
						d.m_val = component(v, ncomp);
						int nf = f.Nodes();
						for (int i=0; i<nf; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
			}
			break;
		case DATA_MAT3F:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<mat3f,DATA_NODE>& df = dynamic_cast<FEFaceData_T<mat3f,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						mat3f m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);

						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;

						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<mat3f,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<mat3f,DATA_ITEM>&>(rd);
					mat3f m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<mat3f,DATA_COMP>& df = dynamic_cast<FEFaceData_T<mat3f,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3f m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<mat3f,DATA_REGION>& dv = dynamic_cast<FEFaceData_T<mat3f,DATA_REGION>&>(rd);
					mat3f m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
			}
			break;
		case DATA_MAT3D:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<mat3d,DATA_NODE>& df = dynamic_cast<FEFaceData_T<mat3d,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						mat3d m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);

						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;

						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<mat3d,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<mat3d,DATA_ITEM>&>(rd);
					mat3d m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<mat3d,DATA_COMP>& df = dynamic_cast<FEFaceData_T<mat3d,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3d m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
			}
			break;
		case DATA_MAT3FS:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<mat3fs,DATA_NODE>& df = dynamic_cast<FEFaceData_T<mat3fs,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						mat3fs m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);

						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;

						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<mat3fs,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<mat3fs,DATA_ITEM>&>(rd);
					mat3fs m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<mat3fs,DATA_COMP>& df = dynamic_cast<FEFaceData_T<mat3fs,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3fs m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<mat3fs,DATA_REGION>& dv = dynamic_cast<FEFaceData_T<mat3fs,DATA_REGION>&>(rd);
					mat3fs m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
			}
			break;
		case DATA_MAT3FD:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<mat3fd,DATA_NODE>& df = dynamic_cast<FEFaceData_T<mat3fd,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						mat3fd m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<mat3fd,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<mat3fd,DATA_ITEM>&>(rd);
					mat3fd m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<mat3fd,DATA_COMP>& df = dynamic_cast<FEFaceData_T<mat3fd,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3fd m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<mat3fd,DATA_REGION>& dv = dynamic_cast<FEFaceData_T<mat3fd,DATA_REGION>&>(rd);
					mat3fd m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
			}
			break;
        case DATA_TENS4FS:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData_T<tens4fs,DATA_NODE>& df = dynamic_cast<FEFaceData_T<tens4fs,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						tens4fs m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
                        
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
                        
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData_T<tens4fs,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<tens4fs,DATA_ITEM>&>(rd);
					tens4fs m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData_T<tens4fs,DATA_COMP>& df = dynamic_cast<FEFaceData_T<tens4fs,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						tens4fs m[4];
						df.eval(n, m);
						d.m_nv[0] = component(m[0], ncomp);
						d.m_nv[1] = component(m[1], ncomp);
						d.m_nv[2] = component(m[2], ncomp);
						d.m_nv[3] = component(m[3], ncomp);
						int nf = f.Nodes();
						d.m_val = 0.f;
						for (int i=0; i<nf; ++i) d.m_val += d.m_nv[i];
						d.m_val /= (float) nf;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData_T<tens4fs,DATA_REGION>& dv = dynamic_cast<FEFaceData_T<tens4fs,DATA_REGION>&>(rd);
					tens4fs m;
					dv.eval(n, &m);
					d.m_val = component(m, ncomp);
					d.m_nv[0] = d.m_nv[1] = d.m_nv[2] = d.m_nv[3] = d.m_val;
					d.m_ntag = 1;
				}
			}
            break;
		}
	}
	else if (IS_NODE_FIELD(nfield))
	{
		int N = f.Nodes();
		NODEDATA n;
		for (int i=0; i<N; ++i) 
		{ 
			EvaluateNode(f.node[i], ntime, nfield, n); 
			d.m_nv[i] = n.m_val;
			d.m_val += d.m_nv[i]; 
		}
		d.m_val /= (float) N;
	}
	else if (IS_ELEM_FIELD(nfield))
	{
		assert((f.m_elem[0] >= 0) && (f.m_elem[0] < m_mesh.Elements()));
		ELEMDATA e;
		EvaluateElement(f.m_elem[0], ntime, nfield, e);
		switch (m_mesh.Element(f.m_elem[0]).m_ntype)
		{
		case FE_TET4:
			{
				const int* fn = FT_TET[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2]) / 3.f;
			}
			break;
        case FE_PENTA6:
            {
                const int* fn = FT_PENTA[f.m_elem[1]];
                switch (f.m_ntype) {
                    case FACE_TRI3:
                        d.m_nv[0] = e.m_nv[fn[0]];
                        d.m_nv[1] = e.m_nv[fn[1]];
                        d.m_nv[2] = e.m_nv[fn[2]];
                        d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2]) / 3.f;
                        break;
                    case FACE_QUAD4:
                        d.m_nv[0] = e.m_nv[fn[0]];
                        d.m_nv[1] = e.m_nv[fn[1]];
                        d.m_nv[2] = e.m_nv[fn[2]];
                        d.m_nv[3] = e.m_nv[fn[3]];
                        d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3]) / 4.f;
                        break;
                    default:
                        break;
                }
            }
            break;
		case FE_HEX8:
			{
				const int* fn = FT_HEX[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_nv[3] = e.m_nv[fn[3]];
				d.m_val = 0.25f*(d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3]);
			}
			break;
		case FE_TET10:
			{
				const int* fn = FT_TET10[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_nv[3] = e.m_nv[fn[3]];
				d.m_nv[4] = e.m_nv[fn[4]];
				d.m_nv[5] = e.m_nv[fn[5]];
				d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5]) / 6.f;
			}
			break;
		case FE_TET15:
			{
				const int* fn = FT_TET15[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_nv[3] = e.m_nv[fn[3]];
				d.m_nv[4] = e.m_nv[fn[4]];
				d.m_nv[5] = e.m_nv[fn[5]];
				d.m_nv[6] = e.m_nv[fn[6]];
				d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6]) / 7.f;
			}
			break;
		case FE_HEX20:
			{
				const int* fn = FT_HEX20[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_nv[3] = e.m_nv[fn[3]];
				d.m_nv[4] = e.m_nv[fn[4]];
				d.m_nv[5] = e.m_nv[fn[5]];
				d.m_nv[6] = e.m_nv[fn[6]];
				d.m_nv[7] = e.m_nv[fn[7]];
				d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])*0.125f;
			}
			break;
		case FE_HEX27:
			{
				const int* fn = FT_HEX27[f.m_elem[1]];
				d.m_nv[0] = e.m_nv[fn[0]];
				d.m_nv[1] = e.m_nv[fn[1]];
				d.m_nv[2] = e.m_nv[fn[2]];
				d.m_nv[3] = e.m_nv[fn[3]];
				d.m_nv[4] = e.m_nv[fn[4]];
				d.m_nv[5] = e.m_nv[fn[5]];
				d.m_nv[6] = e.m_nv[fn[6]];
				d.m_nv[7] = e.m_nv[fn[7]];
				d.m_nv[8] = e.m_nv[fn[8]];
				d.m_val = (d.m_nv[0] + d.m_nv[1] + d.m_nv[2] + d.m_nv[3] + d.m_nv[4] + d.m_nv[5] + d.m_nv[6] + d.m_nv[7])*0.125f;
			}
			break;
		case FE_QUAD4:
		case FE_QUAD8:
		case FE_TRI3:
		case FE_TRI6:
			{
				int nn = f.Nodes();
				d.m_val = 0.0;
				for (int i=0; i<nn; ++i)
				{
					d.m_nv[i] = e.m_nv[i];
					d.m_val += d.m_nv[i];
				}
				d.m_val /= (nn);
			}
			break;
		default:
			assert(false);
		}
	}
	else
	{
		assert(false);
	}
}

//-----------------------------------------------------------------------------
void FEModel::EvaluateElement(int n, int ntime, int nfield, ELEMDATA& d)
{
	m_ntime = ntime;

	// get the element
	FEElement& el = m_mesh.Element(n);
	int ne = el.Nodes();

	// the return value
	d.m_val = 0.f;
	for (int i=0; i<ne; ++i) d.m_nv[i] = 0.f;
	d.m_ntag = 0;

	// get the state
	FEState& state = *m_State[ntime];

	if (IS_ELEM_FIELD(nfield))
	{
		// get the data ID
		int ndata = FIELD_CODE(nfield);
		assert((ndata >= 0) && (ndata < state.m_Data.size()));

		// get the component
		int ncomp = FIELD_COMP(nfield);

		FEMeshData& rd = state.m_Data[ndata];
		Data_Format fmt = rd.GetFormat(); 

		switch (rd.GetType())
		{
		case DATA_FLOAT: 
			{
				if (fmt == DATA_NODE)
				{
					FEElemData_T<float,DATA_NODE>& df = dynamic_cast<FEElemData_T<float,DATA_NODE>&>(rd);
					if (df.active(n))
					{
						df.eval(n, d.m_nv);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) d.m_val += d.m_nv[j];
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_ITEM)
				{
					FEElemData_T<float,DATA_ITEM>& df = dynamic_cast<FEElemData_T<float,DATA_ITEM>&>(rd);
					if (df.active(n))
					{
						df.eval(n, &d.m_val);
						for (int j=0; j<ne; ++j) d.m_nv[j] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<float,DATA_COMP>& df = dynamic_cast<FEElemData_T<float,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						df.eval(n, d.m_nv);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) d.m_val += d.m_nv[j];
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<float,DATA_REGION>& df = dynamic_cast<FEElemData_T<float,DATA_REGION>&>(rd);
					if (df.active(n))
					{
						df.eval(n, &d.m_val);
						for (int j=0; j<ne; ++j) d.m_nv[j] = d.m_val;
						d.m_ntag = 1;
					}
				}
			}
			break;
		case DATA_VEC3F:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FEElemData_T<vec3f,DATA_ITEM>&>(rd);
					if (dv.active(n))
					{
						vec3f v;
						dv.eval(n, &v);
						d.m_val = component(v, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<vec3f,DATA_COMP>& df = dynamic_cast<FEElemData_T<vec3f,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						vec3f v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) 
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<vec3f,DATA_NODE>& dm = dynamic_cast<FEElemData_T<vec3f,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						vec3f v[FEElement::MAX_NODES];
						dm.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<vec3f,DATA_REGION>& dv = dynamic_cast<FEElemData_T<vec3f,DATA_REGION>&>(rd);
					if (dv.active(n))
					{
						vec3f v;
						dv.eval(n, &v);
						d.m_val = component(v, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
			break;
		case DATA_MAT3D:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<mat3d,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3d,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						mat3d m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<mat3d,DATA_NODE>& dm = dynamic_cast<FEElemData_T<mat3d,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						mat3d m[FEElement::MAX_NODES];
						dm.eval(n, m);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(m[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<mat3d,DATA_COMP>& df = dynamic_cast<FEElemData_T<mat3d,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3d v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) 
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<mat3d,DATA_REGION>& dm = dynamic_cast<FEElemData_T<mat3d,DATA_REGION>&>(rd);
					if (dm.active(n))
					{
						mat3d m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
			break;
		case DATA_MAT3F:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<mat3f,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3f,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						mat3f m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<mat3f,DATA_NODE>& dm = dynamic_cast<FEElemData_T<mat3f,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						mat3f m[FEElement::MAX_NODES];
						dm.eval(n, m);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(m[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<mat3f,DATA_COMP>& df = dynamic_cast<FEElemData_T<mat3f,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3f v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) 
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<mat3f,DATA_REGION>& dm = dynamic_cast<FEElemData_T<mat3f,DATA_REGION>&>(rd);
					if (dm.active(n))
					{
						mat3f m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
			break;
		case DATA_MAT3FS:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						mat3fs m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<mat3fs,DATA_NODE>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						mat3fs m[FEElement::MAX_NODES];
						dm.eval(n, m);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(m[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<mat3fs,DATA_COMP>& df = dynamic_cast<FEElemData_T<mat3fs,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3fs v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) 
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<mat3fs,DATA_REGION>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_REGION>&>(rd);
					if (dm.active(n))
					{
						mat3fs m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
			break;
		case DATA_MAT3FD:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<mat3fd,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3fd,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						mat3fd m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<mat3fd,DATA_NODE>& dm = dynamic_cast<FEElemData_T<mat3fd,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						mat3fd m[FEElement::MAX_NODES];
						dm.eval(n, m);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(m[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<mat3fd,DATA_COMP>& df = dynamic_cast<FEElemData_T<mat3fd,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						mat3fd v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j) 
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<mat3fd,DATA_REGION>& dm = dynamic_cast<FEElemData_T<mat3fd,DATA_REGION>&>(rd);
					if (dm.active(n))
					{
						mat3fd m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
			break;		
        case DATA_TENS4FS:
			{
				if (fmt == DATA_ITEM)
				{
					FEElemData_T<tens4fs,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<tens4fs,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						tens4fs m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_NODE)
				{
					FEElemData_T<tens4fs,DATA_NODE>& dm = dynamic_cast<FEElemData_T<tens4fs,DATA_NODE>&>(rd);
					if (dm.active(n))
					{
						tens4fs m[FEElement::MAX_NODES];
						dm.eval(n, m);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(m[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_COMP)
				{
					FEElemData_T<tens4fs,DATA_COMP>& df = dynamic_cast<FEElemData_T<tens4fs,DATA_COMP>&>(rd);
					if (df.active(n))
					{
						tens4fs v[FEElement::MAX_NODES];
						df.eval(n, v);
						d.m_val = 0;
						for (int j=0; j<ne; ++j)
						{
							d.m_nv[j] = component(v[j], ncomp);
							d.m_val += d.m_nv[j];
						}
						d.m_val /= (float) ne;
						d.m_ntag = 1;
					}
				}
				else if (fmt == DATA_REGION)
				{
					FEElemData_T<tens4fs,DATA_REGION>& dm = dynamic_cast<FEElemData_T<tens4fs,DATA_REGION>&>(rd);
					if (dm.active(n))
					{
						tens4fs m;
						dm.eval(n, &m);
						d.m_val = component(m, ncomp);
						for (int i=0; i<ne; ++i) d.m_nv[i] = d.m_val;
						d.m_ntag = 1;
					}
				}
				else
				{
					assert(false);
				}
			}
            break;
		}
	}
	else if (IS_NODE_FIELD(nfield))
	{
		// take the average of the nodal values
		NODEDATA n;
		for (int i=0; i<ne; ++i)
		{ 
			EvaluateNode(el.m_node[i], ntime, nfield, n); 
			d.m_nv[i] = n.m_val;
			d.m_val += d.m_nv[i]; 
		}
		d.m_val /= (float) ne;
	}
	else if (IS_FACE_FIELD(nfield))
	{
		// don't do anything; face data is not copied to the elements
	}
	else
	{
		assert(false);
	}
}

//-----------------------------------------------------------------------------
// Evaluate vector field at node n at time ntime
vec3f FEModel::EvaluateNodeVector(int n, int ntime, int nvec)
{
	m_ntime = ntime;

	vec3f r;
	FEState& state = *m_State[ntime];

	if (IS_NODE_FIELD(nvec))
	{
		// get the data ID
		int ndata = FIELD_CODE(nvec);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) return vec3f(0.f, 0.f, 0.f);

		// get the component
		int ncomp = FIELD_COMP(nvec);

		FEMeshData& rd = state.m_Data[ndata];
		assert(rd.GetFormat() == DATA_ITEM);

		switch (rd.GetType())
		{
		case DATA_VEC3F:
			{
				FENodeData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FENodeData_T<vec3f,DATA_ITEM>&>(rd);
				dv.eval(n, &r);
			}
			break;
		case DATA_MAT3FS:
			{
				FENodeData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3fs,DATA_ITEM>&>(rd);
				mat3fs m;
				dm.eval(n, &m);
				r = m.PrincDirection(ncomp+1);
			}
			break;
		default:
			assert(false);
		}
	}
	else if (IS_ELEM_FIELD(nvec))
	{
		// we take the average of the elements that contain this element
		vector<NodeElemRef>& nel = m_mesh.NodeElemList(n);
		if (!nel.empty())
		{
			for (int i=0; i<(int) nel.size(); ++i) r += EvaluateElemVector(nel[i].first, ntime, nvec);
			r /= (float) nel.size();
		}
	}
	else if (IS_FACE_FIELD(nvec))
	{
		// we take the average of the elements that contain this element
		vector<NodeFaceRef>& nfl = m_mesh.NodeFaceList(n);
		if (!nfl.empty())
		{
			int n = 0;
			vec3f fv;
			for (int i=0; i<(int) nfl.size(); ++i) 
			{
				if (EvaluateFaceVector(nfl[i].first, ntime, nvec, fv)) { r += fv; n++; }
			}
			r /= (float) n;
		}
	}
	else 
	{
		assert(false);
		r = vec3f(0,0,0);
	}

	return r;
}


//-----------------------------------------------------------------------------
// Evaluate face vector data
bool FEModel::EvaluateFaceVector(int n, int ntime, int nvec, vec3f& r)
{
	m_ntime = ntime;

	FEState& state = *m_State[ntime];

	FEFace& f = m_mesh.Face(n);

	if (IS_FACE_FIELD(nvec))
	{
		// get the data ID
		int ndata = FIELD_CODE(nvec);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) { r = vec3f(0.f, 0.f, 0.f); return false; }

		// get the component
		int ncomp = FIELD_COMP(nvec);

		FEMeshData& rd = state.m_Data[ndata];
		Data_Format fmt = rd.GetFormat(); 

		switch (rd.GetType())
		{
		case DATA_VEC3F:
			{
				switch (fmt)
				{
				case DATA_ITEM:
					{
						FEFaceData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FEFaceData_T<vec3f,DATA_ITEM>&>(rd);
						dv.eval(n, &r);
						return true;
					}
					break;
				case DATA_NODE:
					{
						FEFaceData_T<vec3f,DATA_NODE>& dv = dynamic_cast<FEFaceData_T<vec3f,DATA_NODE>&>(rd);
						r = vec3f(0,0,0);
						if (dv.active(n))
						{
							vec3f v[FEFace::MAX_NODES];
							int fn = f.Nodes();
							dv.eval(n, v);
							for (int i=0; i<fn; ++i) r += v[i];
							r /= (float) fn;
						}
						return true;
					}
					break;
				case DATA_COMP:
					{
						vec3f rn[8];
						FEFaceData_T<vec3f,DATA_COMP>& dv = dynamic_cast<FEFaceData_T<vec3f,DATA_COMP>&>(rd);
						r = vec3f(0,0,0);
						if (dv.active(n))
						{
							dv.eval(n, rn);
							int ne = m_mesh.Face(n).Nodes();
							for (int i=0; i<ne; ++i) r += rn[i];
							r /= (float) ne;
							return true;
						}
					}
					break;
				}
			}
			break;
		case DATA_MAT3FS:
			{
				FEFaceData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEFaceData_T<mat3fs,DATA_ITEM>&>(rd);
				mat3fs m;
				dm.eval(n, &m);
				r = m.PrincDirection(ncomp+1);
				return true;
			}
			break;
		default:
			assert(false);
			return false;
		}
	}
	else if (IS_NODE_FIELD(nvec))
	{
		FEFace& f = m_mesh.Face(n);
		// take the average of the nodal values
		for (int i=0; i<f.Nodes(); ++i) r += EvaluateNodeVector(f.node[i], ntime, nvec);
		r /= f.Nodes();
		return true;
	}
	else if (IS_ELEM_FIELD(nvec))
	{
		return false;
	}
	else
	{
		assert(false);
		r = vec3f(0,0,0);
	}

	return false;
}

//-----------------------------------------------------------------------------
// Evaluate element vector data
vec3f FEModel::EvaluateElemVector(int n, int ntime, int nvec)
{
	m_ntime = ntime;

	vec3f r;
	FEState& state = *m_State[ntime];

	if (IS_ELEM_FIELD(nvec))
	{
		// get the data ID
		int ndata = FIELD_CODE(nvec);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) return vec3f(0.f, 0.f, 0.f);

		// get the component
		int ncomp = FIELD_COMP(nvec);

		FEMeshData& rd = state.m_Data[ndata];

		int nfmt  = rd.GetFormat();

		switch (rd.GetType())
		{
		case DATA_VEC3F:
			{
				if (nfmt == DATA_ITEM)
				{
					FEElemData_T<vec3f,DATA_ITEM>& dv = dynamic_cast<FEElemData_T<vec3f,DATA_ITEM>&>(rd);
					if (dv.active(n)) dv.eval(n, &r);
				}
				else if (nfmt == DATA_COMP)
				{
					FEElemData_T<vec3f,DATA_COMP>& dv = dynamic_cast<FEElemData_T<vec3f,DATA_COMP>&>(rd);
					vec3f v[8];
					if (dv.active(n))
					{
						dv.eval(n, v);
						int ne = m_mesh.Element(n).Nodes();
						for (int i=0; i<ne; ++i) r += v[i];
						r /= (float) ne;
					}
				}
			}
			break;
		case DATA_MAT3FS:
			{
				if (nfmt == DATA_ITEM)
				{
					FEElemData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_ITEM>&>(rd);
					if (dm.active(n))
					{
						mat3fs m;
						dm.eval(n, &m);
						r = m.PrincDirection(ncomp+1);
					}
				}
				else if (nfmt == DATA_COMP)
				{
					FEElemData_T<mat3fs,DATA_COMP>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_COMP>&>(rd);
					mat3fs m[8];
					r = vec3f(0,0,0);
					if (dm.active(n))
					{
						dm.eval(n, m);
						int ne = m_mesh.Element(n).Nodes();
						for (int i=0; i<ne; ++i) r += m[i].PrincDirection(ncomp + 1);
						r /= (float) ne;
					}
				}
			}
			break;
		default:
			assert(false);
		}
	}
	else if (IS_NODE_FIELD(nvec))
	{
		FEElement& el = m_mesh.Element(n);
		// take the average of the nodal values
		for (int i=0; i<el.Nodes(); ++i) r += EvaluateNodeVector(el.m_node[i], ntime, nvec);
		r /= el.Nodes();
	}
	else if (IS_FACE_FIELD(nvec))
	{
	}
	else
	{
		assert(false);
		r = vec3f(0,0,0);
	}

	return r;
}

//-----------------------------------------------------------------------------
// Evaluate tensor field at node n
mat3fs FEModel::EvaluateNodeTensor(int n, int ntime, int nten)
{
	m_ntime = ntime;

	FEState& state = *m_State[ntime];
	mat3fs m;

	if (IS_NODE_FIELD(nten))
	{
		// get the data ID
		int ndata = FIELD_CODE(nten);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) return mat3fs();

		FEMeshData& rd = state.m_Data[ndata];

		assert(rd.GetType() == DATA_MAT3FS);

		FENodeData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FENodeData_T<mat3fs,DATA_ITEM>&>(rd);
		dm.eval(n, &m);
	}
	else 
	{
		// we take the average of the elements that contain this element
		vector<NodeElemRef>& nel = m_mesh.NodeElemList(n);
		if (!nel.empty())
		{
			for (int i=0; i<(int) nel.size(); ++i) m += EvaluateElemTensor(nel[i].first, ntime, nten);
			m /= (float) nel.size();
		}
	}

	return m;
}

//-----------------------------------------------------------------------------
// Evaluate tensor field at face n
mat3fs FEModel::EvaluateFaceTensor(int n, int ntime, int nten)
{
	m_ntime = ntime;

	mat3fs m;
	FEState& state = *m_State[ntime];

	if (IS_FACE_FIELD(nten))
	{
		// get the data ID
		int ndata = FIELD_CODE(nten);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) return mat3fs();
		FEMeshData& rd = state.m_Data[ndata];

		assert(rd.GetType() == DATA_MAT3FS);

		FEFaceData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEFaceData_T<mat3fs,DATA_ITEM>&>(rd);
		dm.eval(n, &m);
	}
	else if (IS_NODE_FIELD(nten))
	{
		FEFace& f = m_mesh.Face(n);
		// take the average of the nodal values
		for (int i=0; i<f.Nodes(); ++i) m += EvaluateNodeTensor(f.node[i], ntime, nten);
		m /= (float) f.Nodes();
	}
	else if (IS_ELEM_FIELD(nten))
	{
		
	}
	else
	{
		assert(false);
	}

	return m;
}

//-----------------------------------------------------------------------------
// Evaluate tensor field at element n
mat3fs FEModel::EvaluateElemTensor(int n, int ntime, int nten)
{
	m_ntime = ntime;

	mat3fs m;
	FEState& state = *m_State[ntime];

	if (IS_ELEM_FIELD(nten))
	{
		// get the data ID
		int ndata = FIELD_CODE(nten);
		if ((ndata < 0) || (ndata >= state.m_Data.size())) return mat3fs();
		FEMeshData& rd = state.m_Data[ndata];
		int nfmt  = rd.GetFormat();

		assert(rd.GetType() == DATA_MAT3FS);

		switch (nfmt)
		{
		case DATA_ITEM:
			{
				FEElemData_T<mat3fs,DATA_ITEM>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_ITEM>&>(rd);
				dm.eval(n, &m);
			}
			break;
		case DATA_COMP:
			{
				FEElemData_T<mat3fs,DATA_COMP>& dm = dynamic_cast<FEElemData_T<mat3fs,DATA_COMP>&>(rd);
				mat3fs mi[FEElement::MAX_NODES];
				if (dm.active(n))
				{
					dm.eval(n, mi);
					int ne = m_mesh.Element(n).Nodes();
					m = mi[0];
					for (int i=1; i<ne; ++i) m += mi[i];
					m /= (float) ne;
				}
			};
			break;
		}
	}
	else if (IS_NODE_FIELD(nten))
	{
		FEElement& el = m_mesh.Element(n);
		// take the average of the nodal values
		for (int i=0; i<el.Nodes(); ++i) m += EvaluateNodeTensor(el.m_node[i], ntime, nten);
		m /= (float) el.Nodes();
	}
	else if (IS_FACE_FIELD(nten))
	{
		
	}
	else
	{
		assert(false);
	}

	return m;
}
