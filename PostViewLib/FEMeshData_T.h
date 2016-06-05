#pragma once

#include "FEMeshData.h"
#include "FEModel.h"
#include <set>
using namespace std;

//=============================================================================
// 
//    N O D E   D A T A
// 
//=============================================================================

//-----------------------------------------------------------------------------
// base class for node data
class FENodeItemData : public FEMeshData
{
public:
	FENodeItemData(FEModel* pm, Data_Type ntype, Data_Format nfmt) : FEMeshData(pm, ntype, nfmt){}
};

//-----------------------------------------------------------------------------
// template base class for defining the data value
template <typename T, Data_Format fmt> class FENodeData_T : public FENodeItemData
{
public:
	FENodeData_T(FEModel* pm, FEDataField* pdf) : FENodeItemData(pm, FEMeshDataTraits<T>::Type(), fmt) {}
	virtual void eval(int n, T* pv) = 0;
	virtual bool active(int n) { return true; }

	static Data_Type Type  () { return FEMeshDataTraits<T>::Type  (); }
	static Data_Format Format() { return fmt; }
	static Data_Class Class() { return CLASS_NODE; }
};

//-----------------------------------------------------------------------------
// template class for nodal data stored in vectors
// \todo rename this class to FENodeDataArray or something
template <typename T> class FENodeData : public FENodeData_T<T, DATA_ITEM>
{
public:
	FENodeData(FEModel* pm, FEDataField* pdf) : FENodeData_T<T, DATA_ITEM>(pm, pdf) { m_data.resize(pm->GetMesh()->Nodes()); }
	void eval(int n, T* pv) { (*pv) = m_data[n]; }
	void copy(FENodeData<T>& d) { m_data = d.m_data; }

	T& operator [] (int n) { return m_data[n]; }

protected:
	vector<T>	m_data;
};

//=============================================================================
// 
//    F A C E   D A T A
// 
//=============================================================================

//-----------------------------------------------------------------------------
// base class for face data
class FEFaceItemData : public FEMeshData
{
public:
	FEFaceItemData(FEModel* pm, Data_Type ntype, Data_Format nfmt) : FEMeshData(pm, ntype, nfmt){}
};

//-----------------------------------------------------------------------------
// template base class for defining the data value
template <typename T, Data_Format fmt> class FEFaceData_T : public FEFaceItemData
{
public:
	FEFaceData_T(FEModel* pm, FEDataField* pdf) : FEFaceItemData(pm, FEMeshDataTraits<T>::Type(), fmt) {}
	virtual void eval(int n, T* pv) = 0;
	virtual bool active(int n) { return true; }

	static Data_Type Type  () { return FEMeshDataTraits<T>::Type  (); }
	static Data_Format Format() { return fmt; }
	static Data_Class Class() { return CLASS_FACE; }
};

//-----------------------------------------------------------------------------
// template class for face data stored in vectors
template <typename T, Data_Format fmt> class FEFaceData : public FEFaceData_T<T, fmt> {};

// *** specialization for DATA_ITEM format ***
template <typename T> class FEFaceData<T, DATA_ITEM> : public FEFaceData_T<T, DATA_ITEM>
{
public:
	FEFaceData(FEModel* pm, FEDataField* pdf) : FEFaceData_T<T,DATA_ITEM>(pm, pdf), m_face(pdf->m_item)
	{ 
		if (m_face.empty())
			m_face.assign(pm->GetMesh()->Faces(), -1); 
	}
	void eval(int n, T* pv) { (*pv) = m_data[m_face[n]]; }
	bool active(int n) { return (m_face[n] >= 0); }
	void copy(FEFaceData<T,DATA_ITEM>& d) { m_data = d.m_data; }
	void add(int n, const T& d)
	{ 
		if (m_face[n] >= 0) assert(m_face[n] == (int) m_data.size());
		else m_face[n] = (int) m_data.size(); 
		m_data.push_back(d); 
	}

protected:
	vector<T>		m_data;
	vector<int>&	m_face;
};

// *** specialization for DATA_REGION format ***
template <typename T> class FEFaceData<T, DATA_REGION> : public FEFaceData_T<T, DATA_REGION>
{
public:
	FEFaceData(FEModel* pm, FEDataField* pdf) : FEFaceData_T<T,DATA_REGION>(pm, pdf), m_face(pdf->m_item) 
	{ 
		if (m_face.empty())
			m_face.assign(pm->GetMesh()->Faces(), -1); 
	}
	void eval(int n, T* pv) { (*pv) = m_data; }
	bool active(int n) { return (m_face[n] >= 0); }
	void copy(FEFaceData<T,DATA_ITEM>& d) { m_data = d.m_data; m_face = d.m_face; }
	void add(int n) { m_face[n] = 1; }
	void set(const T& d) { m_data = d; }

protected:
	T				m_data;
	vector<int>&	m_face;
};


// *** specialization for DATA_COMP format ***
template <typename T> class FEFaceData<T, DATA_COMP> : public FEFaceData_T<T, DATA_COMP>
{
public:
	FEFaceData(FEModel* pm, FEDataField* pdf) : FEFaceData_T<T,DATA_COMP>(pm, pdf), m_face(pdf->m_item)
	{ 
		if (m_face.empty())
			m_face.assign(pm->GetMesh()->Faces(), -1); 
	}
	void eval(int n, T* pv)
	{ 
		int m = this->GetFEModel()->GetMesh()->Face(n).Nodes();
		for (int i=0; i<m; ++i) pv[i] = m_data[m_face[n] + i];
	}
	bool active(int n) { return (m_face[n] >= 0); }
	void copy(FEFaceData<T,DATA_COMP>& d) { m_data = d.m_data; m_face = d.m_face; }
	void add(int n, T* d, int m) 
	{ 
		if (m_face[n] >= 0) assert(m_face[n] == (int) m_data.size());
		else m_face[n] = (int) m_data.size();
		for (int i=0; i<m; ++i) m_data.push_back(d[i]);
	}

protected:
	vector<T>		m_data;
	vector<int>&	m_face;
};

// *** specialization for DATA_NODE format ***
// this assumes 4 values per face
template <typename T> class FEFaceData<T, DATA_NODE> : public FEFaceData_T<T, DATA_NODE>
{
public:
	FEFaceData(FEModel* pm, FEDataField* pdf) : FEFaceData_T<T,DATA_NODE>(pm, pdf), m_face(pdf->m_item)
	{
		if (m_face.empty())
		{
			int N = pm->GetMesh()->Faces();
			m_face.assign(4*N, -1); 
		}
	}
	void eval(int n, T* pv)
	{ 
		pv[0] = m_data[m_face[4*n  ]]; 
		pv[1] = m_data[m_face[4*n+1]]; 
		pv[2] = m_data[m_face[4*n+2]]; 
		pv[3] = m_data[m_face[4*n+3]]; 
	}
	bool active(int n) { return (m_face[4*n] >= 0); }
	void copy(FEFaceData<T,DATA_NODE>& d) { m_data = d.m_data; m_face = d.m_face; }
	void add(vector<T>& d, vector<int>& f, vector<int>& l) 
	{ 
		int n0 = m_data.size();
		m_data.insert(m_data.end(), d.begin(), d.end());
		for (int i=0; i<(int) f.size(); ++i) 
		{
			if (m_face[4*f[i]  ] == -1) m_face[4*f[i]  ] = n0 + l[4*i  ]; else assert(m_face[4*f[i]  ] == n0 + l[4*i  ]);
			if (m_face[4*f[i]+1] == -1) m_face[4*f[i]+1] = n0 + l[4*i+1]; else assert(m_face[4*f[i]+1] == n0 + l[4*i+1]);
			if (m_face[4*f[i]+2] == -1) m_face[4*f[i]+2] = n0 + l[4*i+2]; else assert(m_face[4*f[i]+2] == n0 + l[4*i+2]);
			if (m_face[4*f[i]+3] == -1) m_face[4*f[i]+3] = n0 + l[4*i+3]; else assert(m_face[4*f[i]+3] == n0 + l[4*i+3]);
		}
	}

protected:
	vector<T>		m_data;
	vector<int>&	m_face;
};

//=============================================================================
// 
//    E L E M   D A T A
// 
//=============================================================================

//-----------------------------------------------------------------------------
// base class for element data
class FEElemItemData : public FEMeshData
{
public:
	FEElemItemData(FEModel* pm, Data_Type ntype, Data_Format nfmt) : FEMeshData(pm, ntype, nfmt){}
};

//-----------------------------------------------------------------------------
// template base class for defining the data value
template <typename T, Data_Format fmt> class FEElemData_T : public FEElemItemData
{
public:
	FEElemData_T(FEModel* pm, FEDataField* pdf) : FEElemItemData(pm, FEMeshDataTraits<T>::Type(), fmt) {}
	virtual void eval(int n, T* pv) = 0;
	virtual bool active(int n) { return true; }

	static Data_Type Type  () { return FEMeshDataTraits<T>::Type  (); }
	static Data_Format Format() { return fmt; }
	static Data_Class Class() { return CLASS_ELEM; }
};

//-----------------------------------------------------------------------------
// template class for element data stored in vectors
template <typename T, Data_Format fmt> class FEElementData : public FEElemData_T<T, fmt>{};

// *** specialization for DATA_ITEM format ***
template <typename T> class FEElementData<T, DATA_ITEM> : public FEElemData_T<T, DATA_ITEM>
{
public:
	FEElementData(FEModel* pm, FEDataField* pdf) : FEElemData_T<T,DATA_ITEM>(pm, pdf), m_elem(pdf->m_item)
	{ 
		if (m_elem.empty())
			m_elem.assign(pm->GetMesh()->Elements(), -1); 
	}
	void eval(int n, T* pv) { assert(m_elem[n] >= 0); (*pv) = m_data[m_elem[n]]; }
	void set(int n, const T& v) { assert(m_elem[n] >= 0); m_data[m_elem[n]] = v; }
	void copy(FEElementData<T, DATA_ITEM>& d) { m_data = d.m_data; }
	bool active(int n) { return (m_elem[n] >= 0); }
	void add(int n, const T& v)
	{ 
		int m = m_elem[n]; 
		if (m == -1) 
		{ 
			m_elem[n] = m_data.size(); 
		} 
		else assert(m == m_data.size());
		m_data.push_back(v); 
	}
	int size() { return (int) m_data.size(); }
	T& operator [] (int i) { return m_data[i]; }

protected:
	vector<T>		m_data;
	vector<int>&	m_elem;
};

// *** specialization for DATA_REGION format ***
template <typename T> class FEElementData<T, DATA_REGION> : public FEElemData_T<T, DATA_REGION>
{
public:
	FEElementData(FEModel* pm, FEDataField* pdf) : FEElemData_T<T,DATA_REGION>(pm, pdf), m_elem(pdf->m_item)
	{
		if (m_elem.empty())
			m_elem.assign(pm->GetMesh()->Elements(), -1); 
	}
	void eval(int n, T* pv) { assert(m_elem[n] >= 0); (*pv) = m_data; }
	void copy(FEElementData<T, DATA_ITEM>& d) { m_data = d.m_data; m_elem = d.m_elem; }
	bool active(int n) { return (m_elem[n] >= 0); }
	void set(const T& v) { m_data = v; }
	void add(int n) 
	{ 
		int m = m_elem[n]; 
		if (m == -1) { m_elem[n] = 1; } 
	}

protected:
	T				m_data;
	vector<int>&	m_elem;
};

// *** specialization for DATA_COMP format ***
template <typename T> class FEElementData<T, DATA_COMP> : public FEElemData_T<T, DATA_COMP>
{
public:
	FEElementData(FEModel* pm, FEDataField* pdf) : FEElemData_T<T,DATA_COMP>(pm, pdf), m_elem(pdf->m_item) 
	{
		if (m_elem.empty())
		{
			int N = pm->GetMesh()->Elements();
			m_elem.resize(2*N); 
			for (int i=0; i<N; ++i) { m_elem[2*i] = -1; m_elem[2*i+1] = 0; }
		}
	}
	void eval(int i, T* pv)
	{ 
		int n = m_elem[2*i  ];
		int m = m_elem[2*i+1];
		for (int j=0; j<m; ++j) pv[j] = m_data[n + j];
	}
	bool active(int n) { return (m_elem[2*n+1] > 0); }
	void copy(FEElementData<T,DATA_COMP>& d) { m_data = d.m_data; }
	void add(int n, int m, T* d) 
	{ 
		if (m_elem[2*n] == -1)
		{
			m_elem[2*n  ] = (int) m_data.size(); 
			m_elem[2*n+1] = m;
		}
		for (int j=0; j<m; ++j) m_data.push_back(d[j]); 
	}
	int size() { return (int) m_data.size(); }
	T& operator [] (int i) { return m_data[i]; }

protected:
	vector<T>		m_data;
	vector<int>&	m_elem;
};

// *** specialization for DATA_NODE format ***
template <typename T> class FEElementData<T, DATA_NODE> : public FEElemData_T<T, DATA_NODE>
{
public:
	FEElementData(FEModel* pm, FEDataField* pdf) : FEElemData_T<T,DATA_NODE>(pm, pdf), m_elem(pdf->m_item)
	{
		FEMeshBase& m = *pm->GetMesh();
		if (m_elem.empty())
		{
			int N = m.Elements();
			m_elem.resize(2*N);
			for (int i=0; i<N; ++i) { m_elem[2*i] = -1; m_elem[2*i+1] = 0; }
		}
	}
	void eval(int i, T* pv)
	{ 
		int n = m_elem[2*i  ];	// start index in data array
		int m = m_elem[2*i+1];	// size of elem data (should be nr. of nodes)
		for (int j=0; j<m; ++j) pv[j] = m_data[ m_indx[n + j] ];
	}
	bool active(int n) { return (m_elem[2*n] >= 0); }
	void copy(FEElementData<T,DATA_NODE>& d) { m_data = d.m_data; m_indx = d.m_indx; }
	void add(vector<T>& d, vector<int>& e, vector<int>& l, int ne) 
	{ 
		int n0 = m_data.size();
		m_data.insert(m_data.end(), d.begin(), d.end());
		for (int i=0; i<(int) e.size(); ++i) 
		{
			m_elem[2*e[i]  ] = (int) m_indx.size();
			m_elem[2*e[i]+1] = ne;
			for (int j=0; j<ne; ++j) m_indx.push_back(l[i*ne + j] + n0);
		}
	}
	int size() { return (int) m_data.size(); }
	T& operator [] (int i) { return m_data[i]; }

protected:
	vector<T>		m_data;
	vector<int>&	m_elem;
	vector<int>		m_indx;
};

//=============================================================================
// Additional node data fields
//=============================================================================

//-----------------------------------------------------------------------------
class FENodePosition : public FENodeData_T<vec3f, DATA_ITEM>
{
public:
	FENodePosition(FEModel* pm, FEDataField* pdf) : FENodeData_T<vec3f, DATA_ITEM>(pm, pdf){}
	void eval(int n, vec3f* pv);
};

//-----------------------------------------------------------------------------
class FENodeInitPos : public FENodeData_T<vec3f, DATA_ITEM>
{
public:
	FENodeInitPos(FEModel* pm, FEDataField* pdf) : FENodeData_T<vec3f, DATA_ITEM>(pm, pdf){}
	void eval(int n, vec3f* pv);
};

//=============================================================================
// Additional face data fields
//=============================================================================

class FECurvature : public FEFaceData_T<float, DATA_NODE>
{
public:
	FECurvature(FEModel* pm, FEDataField* pdf)  : FEFaceData_T<float, DATA_NODE>(pm, pdf) { m_face.assign(pm->GetMesh()->Faces(), 1);}
	void eval_curvature(int, float* f, int m);

	bool active(int n) { return (m_face[n] == 1); }

	void set_facelist(vector<int>& l);

private:
	void level(int n, int l, set<int>& nl1);

	float nodal_curvature(int n, int m);

public: // parameters
	static int	m_nlevels;	// neighbor search radius
	static int	m_nmax;		// max iterations
	static int  m_bext;		// use extended quadric method

	vector<int> m_face;
};

//-----------------------------------------------------------------------------
class FEGaussCurvature : public FECurvature
{
public:
	FEGaussCurvature(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 0); }
};

//-----------------------------------------------------------------------------
class FEMeanCurvature : public FECurvature
{
public:
	FEMeanCurvature(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 1); }
};

//-----------------------------------------------------------------------------
class FEPrincCurvature1 : public FECurvature
{
public:
	FEPrincCurvature1(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 2); }
};

//-----------------------------------------------------------------------------
class FEPrincCurvature2 : public FECurvature
{
public:
	FEPrincCurvature2(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 3); }
};

//-----------------------------------------------------------------------------
class FERMSCurvature : public FECurvature
{
public:
	FERMSCurvature(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 4); }
};

//-----------------------------------------------------------------------------
class FEDiffCurvature : public FECurvature
{
public:
	FEDiffCurvature(FEModel* pm, FEDataField* pdf) : FECurvature(pm, pdf){}
	void eval(int n, float* f) { FECurvature::eval_curvature(n, f, 5); }
};

//-----------------------------------------------------------------------------
class FEPrincCurvatureVector : public FEFaceData_T<vec3f, DATA_NODE>
{
public:
	FEPrincCurvatureVector(FEModel* pm, FEDataField* pdf)  : FEFaceData_T<vec3f, DATA_NODE>(pm, pdf) { m_face.assign(pm->GetMesh()->Faces(), 1);}

	bool active(int n) { return (m_face[n] == 1); }

	void set_facelist(vector<int>& l);

protected:

	void eval(int n, vec3f* f, int m);

	void level(int n, int l, set<int>& nl1);

	vec3f nodal_curvature(int n, int m);

public: // parameters
	static int	m_nlevels;	// neighbor search radius
	static int	m_nmax;		// max iterations
	static int  m_bext;		// use extended quadric method

	vector<int> m_face;
};

//-----------------------------------------------------------------------------
class FEPrincCurvatureVector1 : public FEPrincCurvatureVector
{
public:
	FEPrincCurvatureVector1(FEModel* pm, FEDataField* pdf) : FEPrincCurvatureVector(pm, pdf){}
	void eval(int n, vec3f* f) { FEPrincCurvatureVector::eval(n, f, 0); }
};

//-----------------------------------------------------------------------------
class FEPrincCurvatureVector2 : public FEPrincCurvatureVector
{
public:
	FEPrincCurvatureVector2(FEModel* pm, FEDataField* pdf) : FEPrincCurvatureVector(pm, pdf){}
	void eval(int n, vec3f* f) { FEPrincCurvatureVector::eval(n, f, 1); }
};

//-----------------------------------------------------------------------------
class FECongruency : public FEFaceData_T<float, DATA_NODE>
{
public:
	FECongruency(FEModel* pm, FEDataField* pdf) : FEFaceData_T<float, DATA_NODE>(pm, pdf) { m_face.assign(pm->GetMesh()->Faces(), 1); }

	bool active(int n) { return (m_face[n] == 1); }

	void set_facelist(vector<int>& l);

protected:
	void eval(int n, float* f);

	vector<int> m_face;

public:
	static int m_nlevels;
	static int m_nmax;
	static int m_bext;
};

//=============================================================================
// Additional element data fields
//=============================================================================

//-----------------------------------------------------------------------------
class FEDeformationGradient : public FEElemData_T<mat3d, DATA_COMP>
{
public:
	FEDeformationGradient(FEModel* pm, FEDataField* pdf);
	void eval(int n, mat3d* pv);
};

//-----------------------------------------------------------------------------
// strain data
class FEStrain
{
public:
	FEStrain() { m_nref = 0; }
public:
	int	m_nref;
};

//-----------------------------------------------------------------------------
class FELagrangeStrain : public FEElemData_T<mat3fs, DATA_COMP>, public FEStrain
{
public:
	FELagrangeStrain(FEModel* pm, FEDataField* pdf);
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FEInfStrain : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FEInfStrain(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FERightCauchyGreen : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FERightCauchyGreen(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FERightStretch : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FERightStretch(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FEGLStrain : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FEGLStrain(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FEBiotStrain : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FEBiotStrain(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FERightHencky : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
	FERightHencky(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
	void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FELeftCauchyGreen : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
    FELeftCauchyGreen(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
    void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FELeftStretch : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
    FELeftStretch(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
    void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FELeftHencky : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
    FELeftHencky(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
    void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FEAlmansi : public FEElemData_T<mat3fs, DATA_ITEM>, public FEStrain
{
public:
    FEAlmansi(FEModel* pm, FEDataField* pdf) : FEElemData_T<mat3fs, DATA_ITEM>(pm, pdf) {}
    void eval(int n, mat3fs* pv);
};

//-----------------------------------------------------------------------------
class FEVolRatio : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEVolRatio(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEElementVolume : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEElementVolume(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEAspectRatio : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEAspectRatio(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEMaxEdgeAngle : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEMaxEdgeAngle(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEMinEdgeAngle : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEMinEdgeAngle(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEVolStrain : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEVolStrain(FEModel* pm, FEDataField* pdf) : FEElemData_T<float, DATA_ITEM>(pm, pdf){}
	void eval(int n, float* pv);
};

//-----------------------------------------------------------------------------
class FEElemPressure : public FEElemData_T<float, DATA_ITEM>
{
public:
	FEElemPressure(FEModel* pm, FEDataField* pdf);
	void eval(int n, float* pv);
private:
	int	m_nstress;	// stress field
};

//-----------------------------------------------------------------------------
// Pressure field corresponding to the "nodal stress" field, which stores for
// each element the stress at the nodes
class FEElemNodalPressure : public FEElemData_T<float, DATA_COMP>
{
public:
	FEElemNodalPressure(FEModel* pm, FEDataField* pdf);
	void eval(int n, float* pv);
private:
	int	m_nstress;	// nodal stress field
};

//-----------------------------------------------------------------------------
class FESolidStress : public FEElemData_T<mat3fs, DATA_ITEM>
{
public:
	FESolidStress(FEModel* pm, FEDataField* pdf);
	void eval(int n, mat3fs* pv);
private:
	int	m_nstress;	// total stress field
	int	m_nflp;		// fluid pressure field
};
