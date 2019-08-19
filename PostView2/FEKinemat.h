#pragma once
#include <list>
#include <MathLib/math3d.h>

class CDocument;

//-----------------------------------------------------------------------------
//! This class implements a tool to apply kinematics data to a model
class FEKinemat
{
protected:
	struct KINE
	{
		float m[16];

		vec3f apply(const vec3f& r);
	};

	class STATE
	{
	public:
		vector<KINE>	D;

	public:
		STATE(){}
		STATE(const STATE& s) { D = s.D; }
		STATE& operator = (const STATE& s) { D = s.D; return (*this); }
	};

public:
	FEKinemat(CDocument* pdoc);

	bool Apply(const char* szfile, const char* szkine);
	void SetRange(int n0, int n1, int ni);

protected:
	bool ReadKine(const char* szkine);
	bool BuildStates();

protected:
	CDocument*	m_pDoc;

	int	m_n0, m_n1, m_ni;

	vector<STATE>	m_State;
};
