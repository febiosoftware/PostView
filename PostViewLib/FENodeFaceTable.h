#pragma once
#include <vector>
using namespace std;

class FEMeshBase;

class FENodeFaceTable
{
public:
	FENodeFaceTable(FEMeshBase* pm);
	int FindFace(int inode, int n[9], int m);

protected:
	void Build();

protected:
	FEMeshBase*	m_pm;
	vector<vector<int> >	m_NFT;
};
