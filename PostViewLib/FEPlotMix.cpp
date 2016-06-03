#include "stdafx.h"
#include "FEPlotMix.h"
#include "FELSDYNAPlot.h"
#include "xpltReader.h"
#include "FEDataManager.h"
#include "FEMeshData_T.h"

//------------------------------------------------------------------------------
// FEPlotMix
//------------------------------------------------------------------------------

FEPlotMix::FEPlotMix(void)
{

}

//------------------------------------------------------------------------------
FEPlotMix::~FEPlotMix(void)
{
}

//------------------------------------------------------------------------------
FEModel* FEPlotMix::Load(const char **szfile, int n)
{
	if (n <= 0) return 0;

	// create the file import
	XpltReader* pfr = new XpltReader;

	// load the first model
	FEModel* pfem = new FEModel;
	pfr->SetReadStateFlag(XpltReader::XPLT_READ_LAST_STATE_ONLY);
	if (pfr->Load(*pfem, szfile[0]) == false) { delete pfem; return 0; }

	// clear all states, except the last one
	ClearStates(*pfem);

	// get the mesh
	FEMeshBase& m1 = *pfem->GetMesh();

	// get the datamanager
	FEDataManager* pdm1 = pfem->GetDataManager();

	// set the time indicator of the first state
	FEState& s1 = *pfem->GetState(0);
	s1.m_time = 0;

	// load the other models
	for (int i=1; i<n; ++i)
	{
		// create a new scene
		FEModel fem2;

		// try to load the scene
		if (pfr->Load(fem2, szfile[i]) == false)
		{
			delete pfem;
			return 0;
		}

		// make sure the mesh size is the same
		FEMeshBase& m2 = *fem2.GetMesh();
		if ((m1.Nodes   () != m2.Nodes()) ||
			(m1.Elements() != m2.Elements())) { delete pfem; return 0; } 

		// clear all the states of the second fem
		ClearStates(fem2);

		// see if the data size is the same
		FEDataManager* pdm2 = fem2.GetDataManager();

		if (pdm1->DataFields() != pdm2->DataFields()) { delete pfem; return 0; }

		// need to modify the displacement field such that it is relative to the first file
		FEState* ps1 = pfem->GetState(0);
		FEState* ps2 = fem2.GetState(0);
		FENodeData<vec3f>& dr2 = dynamic_cast<FENodeData<vec3f>&>(ps2->m_Data[0]);
		for (int j=0; j<m1.Nodes(); ++j)
		{
			FENode& n1 = m1.Node(j);
			FENode& n2 = m2.Node(j);
			dr2[j] = (n2.m_r0 + dr2[j]) - n1.m_r0;
		}

		// add a new state by copying it from fem2
		FEState* pstate = new FEState((float) i, pfem, fem2.GetState(0));
		pfem->AddState(pstate);
	}

	// cleanup
	delete pfr;

	return pfem;
}

//------------------------------------------------------------------------------
void FEPlotMix::ClearStates(FEModel &fem)
{
	while (fem.GetStates() > 1) fem.DeleteState(0);
}
