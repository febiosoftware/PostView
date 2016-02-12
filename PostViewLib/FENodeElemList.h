#pragma once
#include "FEElement.h"
#include <utility>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------
// Forward declaration of the mesh class
class FEMesh;

//-----------------------------------------------------------------------------
// Defintion of the NodeFaceRef
// the first index is the element number
// the second index is the local node index of the element
typedef pair<int, short>	NodeElemRef;

//-----------------------------------------------------------------------------
// For each node of the mesh stores a list of elements that connect to that node
class FENodeElemList
{
public:
	FENodeElemList(void) { m_pm = 0; }

	void Build(FEMesh* pm);

	void Clear() { m_NEL.clear(); }

	vector<NodeElemRef>& ElemList(int n) { return m_NEL[n]; }

	bool Empty() { return m_NEL.empty(); }

protected:
	FEMesh*	m_pm;
	vector<vector<NodeElemRef> >	m_NEL;
};
