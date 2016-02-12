#pragma once
#include <vector>
using namespace std;

class FEMesh;

class FENodeFaceTable
{
public:
	FENodeFaceTable(FEMesh* pm);
	int FindFace(int inode, int n[9], int m);

protected:
	void Build();

protected:
	FEMesh*	m_pm;
	vector<vector<int> >	m_NFT;
};
