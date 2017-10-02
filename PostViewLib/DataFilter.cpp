#include "stdafx.h"
#include "DataFilter.h"
#include "FEModel.h"
#include "constants.h"
#include "FEMeshData_T.h"

// TODO: Find a way to return errors
void DataScale(FEModel& fem, int nfield, double scale)
{
	FEMeshBase& mesh = *fem.GetFEMesh(0);
	float fscale = (float) scale;
	// loop over all states
	int NN = mesh.Nodes();
	int ndata = FIELD_CODE(nfield);
	for (int i = 0; i<fem.GetStates(); ++i)
	{
		FEState& s = *fem.GetState(i);
		FEMeshData& d = s.m_Data[ndata];
		Data_Type type = d.GetType();
		Data_Format fmt = d.GetFormat();
		if (IS_NODE_FIELD(nfield))
		{
			switch (type)
			{
			case DATA_FLOAT:
			{
				FENodeData<float>* pf = dynamic_cast< FENodeData<float>* >(&d);
				for (int n = 0; n<NN; ++n) { float& v = (*pf)[n]; v *= fscale; }
			}
			break;
			case DATA_VEC3F:
			{
				FENodeData<vec3f>* pv = dynamic_cast< FENodeData<vec3f>* >(&d);
				for (int n = 0; n<NN; ++n) { vec3f& v = (*pv)[n]; v *= fscale; }
			}
			break;
			case DATA_MAT3FS:
			{
				FENodeData<mat3fs>* pv = dynamic_cast< FENodeData<mat3fs>* >(&d);
				for (int n = 0; n<NN; ++n) { mat3fs& v = (*pv)[n]; v *= fscale; }
			}
			break;
			case DATA_MAT3D:
			{
				FENodeData<mat3d>* pv = dynamic_cast< FENodeData<mat3d>* >(&d);
				for (int n = 0; n<NN; ++n) { mat3d& v = (*pv)[n]; v *= fscale; }
			}
			break;
			case DATA_MAT3F:
			{
				FENodeData<mat3f>* pv = dynamic_cast< FENodeData<mat3f>* >(&d);
				for (int n = 0; n<NN; ++n) { mat3f& v = (*pv)[n]; v *= fscale; }
			}
			break;
			default:
				break;
			}
		}
		else if (IS_ELEM_FIELD(nfield))
		{
			switch (type)
			{
			case DATA_FLOAT:
			{
				if (fmt == DATA_NODE)
				{
					FEElementData<float, DATA_NODE>* pf = dynamic_cast<FEElementData<float, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEElementData<float, DATA_ITEM>* pf = dynamic_cast<FEElementData<float, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEElementData<float, DATA_COMP>* pf = dynamic_cast<FEElementData<float, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEElementData<float, DATA_REGION>* pf = dynamic_cast<FEElementData<float, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_VEC3F:
			{
				if (fmt == DATA_NODE)
				{
					FEElementData<vec3f, DATA_NODE>* pf = dynamic_cast<FEElementData<vec3f, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEElementData<vec3f, DATA_ITEM>* pf = dynamic_cast<FEElementData<vec3f, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEElementData<vec3f, DATA_COMP>* pf = dynamic_cast<FEElementData<vec3f, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEElementData<vec3f, DATA_REGION>* pf = dynamic_cast<FEElementData<vec3f, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_MAT3FS:
			{
				if (fmt == DATA_NODE)
				{
					FEElementData<mat3fs, DATA_NODE>* pf = dynamic_cast<FEElementData<mat3fs, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEElementData<mat3fs, DATA_ITEM>* pf = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEElementData<mat3fs, DATA_COMP>* pf = dynamic_cast<FEElementData<mat3fs, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEElementData<mat3fs, DATA_REGION>* pf = dynamic_cast<FEElementData<mat3fs, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_MAT3D:
			{
				if (fmt == DATA_NODE)
				{
					FEElementData<mat3d, DATA_NODE>* pf = dynamic_cast<FEElementData<mat3d, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEElementData<mat3d, DATA_ITEM>* pf = dynamic_cast<FEElementData<mat3d, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEElementData<mat3d, DATA_COMP>* pf = dynamic_cast<FEElementData<mat3d, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEElementData<mat3d, DATA_REGION>* pf = dynamic_cast<FEElementData<mat3d, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_MAT3F:
			{
				if (fmt == DATA_NODE)
				{
					FEElementData<mat3f, DATA_NODE>* pf = dynamic_cast<FEElementData<mat3f, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEElementData<mat3f, DATA_ITEM>* pf = dynamic_cast<FEElementData<mat3f, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEElementData<mat3f, DATA_COMP>* pf = dynamic_cast<FEElementData<mat3f, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEElementData<mat3f, DATA_REGION>* pf = dynamic_cast<FEElementData<mat3f, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			default:
				return;
				break;
			}
		}
		else if (IS_FACE_FIELD(nfield))
		{
			switch (type)
			{
			case DATA_FLOAT:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData<float, DATA_NODE>* pf = dynamic_cast<FEFaceData<float, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData<float, DATA_ITEM>* pf = dynamic_cast<FEFaceData<float, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData<float, DATA_COMP>* pf = dynamic_cast<FEFaceData<float, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData<float, DATA_REGION>* pf = dynamic_cast<FEFaceData<float, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n=0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_VEC3F:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData<vec3f, DATA_NODE>* pf = dynamic_cast<FEFaceData<vec3f, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData<vec3f, DATA_ITEM>* pf = dynamic_cast<FEFaceData<vec3f, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData<vec3f, DATA_COMP>* pf = dynamic_cast<FEFaceData<vec3f, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData<vec3f, DATA_REGION>* pf = dynamic_cast<FEFaceData<vec3f, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_MAT3FS:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData<mat3fs, DATA_NODE>* pf = dynamic_cast<FEFaceData<mat3fs, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData<mat3fs, DATA_ITEM>* pf = dynamic_cast<FEFaceData<mat3fs, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData<mat3fs, DATA_COMP>* pf = dynamic_cast<FEFaceData<mat3fs, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData<mat3fs, DATA_REGION>* pf = dynamic_cast<FEFaceData<mat3fs, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			case DATA_MAT3D:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData<mat3d, DATA_NODE>* pf = dynamic_cast<FEFaceData<mat3d, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= scale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData<mat3d, DATA_ITEM>* pf = dynamic_cast<FEFaceData<mat3d, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= scale;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData<mat3d, DATA_COMP>* pf = dynamic_cast<FEFaceData<mat3d, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= scale;
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData<mat3d, DATA_REGION>* pf = dynamic_cast<FEFaceData<mat3d, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= scale;
				}
			}
			break;
			case DATA_MAT3F:
			{
				if (fmt == DATA_NODE)
				{
					FEFaceData<mat3f, DATA_NODE>* pf = dynamic_cast<FEFaceData<mat3f, DATA_NODE>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= (float) scale;
				}
				else if (fmt == DATA_ITEM)
				{
					FEFaceData<mat3f, DATA_ITEM>* pf = dynamic_cast<FEFaceData<mat3f, DATA_ITEM>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= (float) scale;
				}
				else if (fmt == DATA_COMP)
				{
					FEFaceData<mat3f, DATA_COMP>* pf = dynamic_cast<FEFaceData<mat3f, DATA_COMP>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= (float) scale;
				}
				else if (fmt == DATA_REGION)
				{
					FEFaceData<mat3f, DATA_REGION>* pf = dynamic_cast<FEFaceData<mat3f, DATA_REGION>*>(&d);
					int N = pf->size();
					for (int n = 0; n<N; ++n) (*pf)[n] *= fscale;
				}
			}
			break;
			default:
				return;
				break;
			}
		}
		else 
		{
			break; 
		}
	}
}

//-----------------------------------------------------------------------------
// Apply a smoothing step operation on data
bool DataSmoothStep(FEModel& fem, int nfield, double theta)
{
	// loop over all states
	int ndata = FIELD_CODE(nfield);
	for (int n = 0; n<fem.GetStates(); ++n)
	{
		FEState& s = *fem.GetState(n);
		FEMeshBase& mesh = *s.GetFEMesh();
		if (IS_NODE_FIELD(nfield))
		{
			int NN = mesh.Nodes();
			FEMeshData& d = s.m_Data[ndata];
			
			switch (d.GetType())
			{
			case DATA_FLOAT:
			{
				vector<float> D; D.assign(NN, 0.f);
				vector<int> tag; tag.assign(NN, 0);
				FENodeData<float>& data = dynamic_cast< FENodeData<float>& >(d);

				// evaluate the average value of the neighbors
				int NE = mesh.Elements();
				for (int i=0; i<NE; ++i)
				{
					FEElement& el = mesh.Element(i);
					int ne = el.Nodes();
					for (int j=0; j<ne; ++j)
					{
						float f = data[el.m_node[j]];
						for (int k = 0; k<ne; ++k)
						if (k != j)
						{
							int nk = el.m_node[k];
							D[nk] += f;
							tag[nk]++;
						}
					}
				}

				// normalize 
				for (int i=0; i<NN; ++i) if (tag[i]>0) D[i] /= (float) tag[i];

				// assign to data field
				for (int i = 0; i<NN; ++i) { data[i] = (1.f - theta)*data[i] + theta*D[i];  }
			}
			break;
			case DATA_VEC3F:
			{
				vector<vec3f> D; D.assign(NN, vec3f(0.f, 0.f, 0.f));
				vector<int> tag; tag.assign(NN, 0);
				FENodeData<vec3f>& data = dynamic_cast< FENodeData<vec3f>& >(d);

				// evaluate the average value of the neighbors
				int NE = mesh.Elements();
				for (int i = 0; i<NE; ++i)
				{
					FEElement& el = mesh.Element(i);
					int ne = el.Nodes();
					for (int j = 0; j<ne; ++j)
					{
						vec3f v = data[el.m_node[j]];
						for (int k = 0; k<ne; ++k)
						if (k != j)
						{
							int nk = el.m_node[k];
							D[nk] += v;
							tag[nk]++;
						}
					}
				}

				// normalize 
				for (int i = 0; i<NN; ++i) if (tag[i]>0) D[i] /= (float)tag[i];

				// assign to data field
				for (int i = 0; i<NN; ++i) { data[i] = data[i] * (1.f - theta) + D[i]*theta; }
			}
			break;
			default:
				return false;
			}
		}
		else if (IS_ELEM_FIELD(nfield))
		{
			FEMeshData& d = s.m_Data[ndata];
			if ((d.GetFormat() == DATA_ITEM)&&(d.GetType() == DATA_FLOAT))
			{
				int NE = mesh.Elements();

				vector<float> D; D.assign(NE, 0.f);
				vector<int> tag; tag.assign(NE, 0);
				FEElementData<float, DATA_ITEM>& data = dynamic_cast< FEElementData<float, DATA_ITEM>& >(d);

				for (int i=0; i<NE; ++i) mesh.Element(i).m_ntag = i;

				// evaluate the average value of the neighbors
				for (int i=0; i<NE; ++i)
				{
					FEElement& el = mesh.Element(i);
					int nf = el.Faces();
					for (int j=0; j<nf; ++j)
					{
						FEElement* pj = el.m_pElem[j];
						if (pj && (data.active(pj->m_ntag)))
						{
							float f;
							data.eval(pj->m_ntag, &f);
							D[i] += f;
							tag[i]++;
						}
					}
				}

				// normalize 
				for (int i=0; i<NE; ++i) if (tag[i]>0) D[i] /= (float) tag[i];

				// assign to data field
				for (int i = 0; i<NE; ++i) 
					if (data.active(i))
					{
						float f;
						data.eval(i, &f);
						D[i] = (1.f - theta)*f + theta*D[i];
						data.set(i, D[i]);
					}
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Apply a smoothing operation on data
void DataSmooth(FEModel& fem, int nfield, double theta, int niters)
{
	for (int n = 0; n<niters; ++n) 
	{
		if (DataSmoothStep(fem, nfield, theta) == false) break;
	}
}

//-----------------------------------------------------------------------------
// functions used in arithemtic filters
double flt_add(double d, double s) { return d+s; }
double flt_sub(double d, double s) { return d-s; }
double flt_mul(double d, double s) { return d*s; }
double flt_div(double d, double s) { return d/s; }
double flt_err(double d, double s) { return fabs(d - s); }

//-----------------------------------------------------------------------------
void DataArithmetic(FEModel& fem, int nfield, int nop, int noperand)
{
	int ndst = FIELD_CODE(nfield);
	int nsrc = FIELD_CODE(noperand);

	// loop over all states
	for (int n = 0; n<fem.GetStates(); ++n)
	{
		FEState& state = *fem.GetState(n);
		FEMeshData& d = state.m_Data[ndst];
		FEMeshData& s = state.m_Data[nsrc];

		Data_Format fmt = d.GetFormat();
		if (d.GetFormat() != s.GetFormat()) return;
		if ((d.GetType() != s.GetType()) && (s.GetType() != DATA_FLOAT)) return;

		if (IS_NODE_FIELD(nfield) && IS_NODE_FIELD(noperand))
		{
			if ((d.GetType() == DATA_FLOAT) && (s.GetType() == DATA_FLOAT))
			{
				double(*f)(double, double) = 0;
				if      (nop == 0) f = flt_add;
				else if (nop == 1) f = flt_sub;
				else if (nop == 2) f = flt_mul;
				else if (nop == 3) f = flt_div;
				else if (nop == 4) f = flt_err;
				else
				{
					return;
				}

				FENodeData<float>* pd = dynamic_cast<FENodeData<float>*>(&d);
				FENodeData<float>* ps = dynamic_cast<FENodeData<float>*>(&s);
				int N = pd->size();
				for (int i = 0; i<N; ++i) (*pd)[i] = f((*pd)[i], (*ps)[i]);
			}
			else if (d.GetType() == DATA_VEC3F)
			{
				if (s.GetType() == DATA_VEC3F)
				{
					FENodeData<vec3f>* pd = dynamic_cast<FENodeData<vec3f>*>(&d);
					FENodeData<vec3f>* ps = dynamic_cast<FENodeData<vec3f>*>(&s);
					int N = pd->size();
					switch (nop)
					{
					case 0: for (int i = 0; i<N; ++i) (*pd)[i] += (*ps)[i]; break;
					case 1: for (int i = 0; i<N; ++i) (*pd)[i] -= (*ps)[i]; break;
					}
				}
				else if (s.GetType() == DATA_FLOAT)
				{
					FENodeData<vec3f>* pd = dynamic_cast<FENodeData<vec3f>*>(&d);
					FENodeData<float>* ps = dynamic_cast<FENodeData<float>*>(&s);
					int N = pd->size();
					switch (nop)
					{
					case 2: for (int i = 0; i<N; ++i) (*pd)[i] *= (*ps)[i]; break;
					case 3: for (int i = 0; i<N; ++i) (*pd)[i] /= (*ps)[i]; break;
					}
				}
			}
		}
		else if (IS_ELEM_FIELD(nfield) && IS_ELEM_FIELD(noperand))
		{
			if ((d.GetType() == DATA_FLOAT) && (s.GetType() == DATA_FLOAT))
			{
				double (*f)(double,double) = 0;
				if      (nop == 0) f = flt_add;
				else if (nop == 1) f = flt_sub;
				else if (nop == 2) f = flt_mul;
				else if (nop == 3) f = flt_div;
				else if (nop == 4) f = flt_err;
				else
				{
					return;
				}

				if (fmt == DATA_ITEM)
				{
					FEElementData<float, DATA_ITEM>* pd = dynamic_cast<FEElementData<float, DATA_ITEM>*>(&d);
					FEElementData<float, DATA_ITEM>* ps = dynamic_cast<FEElementData<float, DATA_ITEM>*>(&s);
					if (pd && ps)
					{
						int N = pd->size();
						for (int i = 0; i<N; ++i) (*pd)[i] = f((*pd)[i], (*ps)[i]); break;
					}
				}
				else
				{
					return;
				}
			}
			else if (d.GetType() == DATA_MAT3FS)
			{
				if (s.GetType() == DATA_MAT3FS)
				{
					if (fmt == DATA_ITEM)
					{
						FEElementData<mat3fs, DATA_ITEM>* pd = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>*>(&d);
						FEElementData<mat3fs, DATA_ITEM>* ps = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>*>(&s);
						if (pd && ps)
						{
							int N = pd->size();
							switch (nop)
							{
							case 0: for (int i = 0; i<N; ++i) (*pd)[i] += (*ps)[i]; break;
							case 1: for (int i = 0; i<N; ++i) (*pd)[i] -= (*ps)[i]; break;
							default:
								{
									return;
								}
							}
						}
					}
				}
				else if (s.GetType() == DATA_FLOAT)
				{
					if (fmt == DATA_ITEM)
					{
						FEElementData<mat3fs, DATA_ITEM>* pd = dynamic_cast<FEElementData<mat3fs, DATA_ITEM>*>(&d);
						FEElementData<float, DATA_ITEM>* ps = dynamic_cast<FEElementData<float, DATA_ITEM>*>(&s);
						if (pd && ps)
						{
							int N = pd->size();
							switch (nop)
							{
							case 2: for (int i = 0; i<N; ++i) (*pd)[i] *= (*ps)[i]; break;
							case 3: for (int i = 0; i<N; ++i) (*pd)[i] /= (*ps)[i]; break;
							default:
								{
									return;
								}
							}
						}
					}
				}
				else
				{
					return;
				}
			}
		}
		else
		{

			return;
		}
	}
}

//-----------------------------------------------------------------------------
void DataGradient(FEModel& fem, int vecField, int sclField)
{
	int nvec = FIELD_CODE(vecField);
	int nscl = FIELD_CODE(sclField);

	// loop over all the states
	for (int n=0; n<fem.GetStates(); ++n)
	{
		FEState& state = *fem.GetState(n);
		FEMeshData& v = state.m_Data[nvec];
		FEMeshData& s = state.m_Data[nscl];

		// zero the vector field
		if (IS_NODE_FIELD(vecField) && (v.GetType() == DATA_VEC3F))
		{
			FENodeData<vec3f>* pv = dynamic_cast<FENodeData<vec3f>*>(&v);
			int N = pv->size();
			for (int i = 0; i<N; ++i) (*pv)[i] = vec3f(0,0,0);
		}
		else return;

		// get the mesh
		FEMeshBase* mesh = state.GetFEMesh();

		// evaluate the field over all the nodes
		const int NN = mesh->Nodes();
		vector<double> d(NN, 0.f);

		if (s.GetType() == DATA_FLOAT)
		{
			if (IS_NODE_FIELD(sclField))
			{
				FENodeData<float>* ps = dynamic_cast<FENodeData<float>*>(&s);
				for (int i=0; i<NN; ++i) d[i] = (*ps)[i];
			}
			else if (IS_ELEM_FIELD(sclField))
			{
				if (s.GetFormat() == DATA_NODE)
				{
					vector<int> tag(NN, 0);
					FEElementData<float, DATA_NODE>* ps = dynamic_cast<FEElementData<float, DATA_NODE>*>(&s);

					float ed[FEGenericElement::MAX_NODES] = {0.f};
					for (int i=0; i<mesh->Elements(); ++i)
					{
						FEElement& el = mesh->Element(i);
						if (ps->active(i))
						{
							ps->eval(i, ed);
							for (int j=0; j<el.Nodes(); ++j)
							{
								d[el.m_node[j]] += ed[j];
								tag[el.m_node[j]]++;
							}
						}
					}
					for (int i=0; i<NN; ++i)
						if (tag[i] > 0) d[i] /= (double) tag[i];
				}
			}
		}

		// now, calculate the gradient for each element
		vector<vec3f> G(NN, vec3f(0.f, 0.f, 0.f));
		vec3f eg[FEGenericElement::MAX_NODES];
		float ed[FEGenericElement::MAX_NODES];
		vector<int> tag(NN, 0);
		for (int i=0; i<mesh->Elements(); ++i)
		{
			FEElement& el = mesh->Element(i);

			for (int j = 0; j<el.Nodes(); ++j) ed[j] = d[el.m_node[j]];

			for (int j=0; j<el.Nodes(); ++j)
			{
				// get the iso-coords at the nodes
				double q[3] = {0,0,0};
				el.iso_coord(j, q);

				// evaluate the gradient at the node
				shape_grad(fem, i, q, n, eg);

				vec3f grad(0.f, 0.f, 0.f);
				for (int k=0; k<el.Nodes(); ++k) grad += eg[k] * ed[k];
				
				G[el.m_node[j]] += grad;
				tag[el.m_node[j]]++;
			}
		}

		FENodeData<vec3f>* pv = dynamic_cast<FENodeData<vec3f>*>(&v);
		for (int i = 0; i<NN; ++i)
		{
			if (tag[i] > 0) G[i] /= (float) tag[i];
			(*pv)[i] = G[i];
		}
	}
}
