#pragma once
#include "FEElement.h"
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
// forward declaration of the mesh class
class FEMesh;

//-----------------------------------------------------------------------------
// Base class that describes a group of mesh items. 
class FEGroup
{
public:
	FEGroup(FEMesh* pm) { m_pm = pm; m_szname[0] = 0; }
	virtual ~FEGroup(void) {}

	const char* GetName();
	void SetName(const char* szname);

	FEMesh* GetMesh() const { return m_pm; }

protected:
	FEMesh*	m_pm;	// pointer to the parent mesh
	char	m_szname[64];
};

//-----------------------------------------------------------------------------
// A doman is an internal organization of elements. A domain is created for each material.
class FEDomain
{
public:
	FEDomain(FEMesh* pm, int nmat);

	int Faces() { return (int) m_Face.size(); }
	FEFace& Face(int n);

	int Elements() { return (int) m_Elem.size(); }
	FEElement& Element(int n);

protected:
	FEMesh*	m_pm;
	int			m_nmat;	// material index
	vector<int>	m_Face;	// face indices 
	vector<int>	m_Elem;	// element indices
};

//-----------------------------------------------------------------------------
// Class that describes a group of elements
class FEPart : public FEGroup
{
public:
	FEPart(FEMesh* pm) : FEGroup(pm) {}

	int Size() const { return (int) m_Elem.size(); }

	void GetNodeList(vector<int>& node, vector<int>& lnode);

public:
	vector<int>	m_Elem;	// element indices
};

//-------------------------------------------------------------------------
// Class that describes a group of faces
class FESurface : public FEGroup
{
public:
	FESurface(FEMesh* pm) : FEGroup(pm) {}

	int Size() const { return (int) m_Face.size(); }

	void GetNodeList(vector<int>& node, vector<int>& lnode);

public:
	vector<int>	m_Face;	// face indices
};

//-------------------------------------------------------------------------
//! Class that defines a node set
class FENodeSet : public FEGroup
{
public:
	FENodeSet(FEMesh* pm) : FEGroup(pm){}

public:
	vector<int>	m_Node;
};
