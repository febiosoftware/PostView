#include "stdafx.h"
#include "FENodeFaceTable.h"
#include "FEMesh.h"

FENodeFaceTable::FENodeFaceTable(FEMesh* pm)
{
	m_pm = pm;
	Build();
}

void FENodeFaceTable::Build()
{
	FEMesh& m = *m_pm;

	int NF = m.Faces();
	int NN = m.Nodes();
	m_NFT.resize(NN);
	for (int i=0; i<NF; ++i)
	{
		FEFace& f = m.Face(i);
		int nf = f.Nodes();
		for (int j=0; j<nf; ++j)
		{
			vector<int>& nj = m_NFT[f.node[j]];
			nj.push_back(i);
		}
	}
}

int FENodeFaceTable::FindFace(int inode, int n[9], int m)
{
	FEFace ft;
	for (int i=0; i<m; ++i) ft.node[i] = n[i];
	switch (m)
	{
	case 3: ft.m_ntype = FACE_TRI3 ; break;
	case 4: ft.m_ntype = FACE_QUAD4; break;
	case 6: ft.m_ntype = FACE_TRI6 ; break;
	case 7: ft.m_ntype = FACE_TRI7 ; break;
	case 8: ft.m_ntype = FACE_QUAD8; break;
	case 9: ft.m_ntype = FACE_QUAD9; break;
	};
	
	vector<int>& ni = m_NFT[inode];
	int nf = ni.size();
	for (int i=0; i<nf; ++i)
	{
		FEFace& f = m_pm->Face(ni[i]);
		if (f == ft) return ni[i];
	}
	return -1;
}
