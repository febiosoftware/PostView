#include "stdafx.h"
#include "FEMathData.h"

FEMathData::FEMathData(FEState* state, FEMathDataField* pdf) : FENodeData_T<float>(state, pdf)
{
	m_pdf = pdf;
}

// evaluate all the nodal data for this state
void FEMathData::eval(int n, float* pv)
{
	FEModel& fem = *GetFEModel();

	int ntime = fem.currentTime();
	FEState& state = *fem.GetState(ntime);
	double time = (double)state.m_time;

	FEMeshBase& mesh = *state.GetFEMesh();

	CMathParser math;
	math.set_variable("t", time);

	int ierr;
	FENode& node = mesh.Node(n);

	vec3f r = fem.NodePosition(n, ntime);
	math.set_variable("x", (double)r.x);
	math.set_variable("y", (double)r.y);
	math.set_variable("z", (double)r.z);

	const std::string& eq = m_pdf->EquationString();

	double v = math.eval(eq.c_str(), ierr);

	if (pv) *pv = (float) v;
}
