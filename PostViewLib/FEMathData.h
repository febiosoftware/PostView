#pragma once
#include "FEMeshData_T.h"
#include "MathParser.h"

class FEMathDataField;

class FEMathData : public FENodeData_T<float>
{
public:
	FEMathData(FEState* state, FEMathDataField* pdf);

	// evaluate the nodal data for this state
	void eval(int n, float* pv) override;

private:
	FEMathDataField*	m_pdf;
};

class FEMathDataField : public FEDataField
{
public:
	FEMathDataField(const std::string& name, unsigned int flag = 0) : FEDataField(name, DATA_FLOAT, DATA_NODE, CLASS_NODE, flag)
	{
		m_eq = "";
	}

	//! Create a copy
	FEDataField* Clone() const override
	{
		FEMathDataField* pd = new FEMathDataField(GetName());
		pd->m_eq = m_eq;
		return pd;
	}

	//! FEMeshData constructor
	FEMeshData* CreateData(FEState* pstate) override
	{
		return new FEMathData(pstate, this);
	}

	void SetEquationString(const std::string& eq) { m_eq = eq; }

	const std::string& EquationString() const { return m_eq; }

private:
	std::string	m_eq;		//!< equation string
};
