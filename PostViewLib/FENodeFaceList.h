#pragma once
#include "FEElement.h"
#include <vector>
#include <utility>
using namespace std;

//-----------------------------------------------------------------------------
// Forward declaration of the mesh class
class FEMesh;

//-----------------------------------------------------------------------------
// Defintion of the NodeFaceRef
// the first index is the face number
// the second index is the local node index of the face
typedef pair<int, short>	NodeFaceRef;

//-----------------------------------------------------------------------------
// For each node of the mesh stores a list of faces that connect to that node
class FENodeFaceList
{
public:
	FENodeFaceList(void) { m_pm = 0; }

	void Build(FEMesh* pm);

	void Clear() { m_NFL.clear(); }

	vector<NodeFaceRef>& FaceList(int n) { return m_NFL[n]; }

	bool Empty() { return m_NFL.empty(); }

protected:
	FEMesh*	m_pm;
	vector<vector<NodeFaceRef> >	m_NFL;
};
