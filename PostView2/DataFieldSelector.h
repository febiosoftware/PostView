#pragma once
#include <QComboBox>
#include <PostViewLib/FEMeshData.h>
#include <PostViewLib/FEModel.h>

class QTreeWidgetItem;

class CDataFieldSelector : public QComboBox, public FEModelDependant
{
public:
	CDataFieldSelector(QWidget* parent = 0);
	~CDataFieldSelector();

	void BuildMenu(FEModel* fem, Data_Tensor_Type ntype, bool btvec = false);

	int currentValue() const;

	void setCurrentValue(int n);

public:
	// from FEModelDependant
	void Update(FEModel* pfem);

private:
	void addComponent(QTreeWidgetItem* parent, const char* szname, int ndata);
	FEModel*	m_fem;
	Data_Tensor_Type	m_class;
	bool m_bvec;
};
