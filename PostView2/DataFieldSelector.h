#pragma once
#include <QComboBox>
#include <PostViewLib/FEMeshData.h>

class FEModel;
class QTreeWidgetItem;

class CDataFieldSelector : public QComboBox
{
public:
	CDataFieldSelector(QWidget* parent = 0);

	void BuildMenu(FEModel* fem, Data_Tensor_Type ntype, bool btvec = false);

	int currentValue() const;

	void setCurrentValue(int n);

private:
	void addComponent(QTreeWidgetItem* parent, const char* szname, int ndata);
};
