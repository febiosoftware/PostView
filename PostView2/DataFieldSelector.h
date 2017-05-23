#pragma once
#include <QComboBox>
#include <PostViewLib/FEMeshData.h>
#include <PostViewLib/FEModel.h>

class QTreeWidgetItem;
class QTreeWidget;

class CDataFieldSelector : public QComboBox, public FEModelDependant
{
	Q_OBJECT

public:
	CDataFieldSelector(QWidget* parent = 0);
	~CDataFieldSelector();

	void BuildMenu(FEModel* fem, Data_Tensor_Type ntype, bool btvec = false);

	int currentValue() const;

	void setCurrentValue(int n);

public:
	// from FEModelDependant
	void Update(FEModel* pfem);

private slots:
	void onItemEntered(QTreeWidgetItem*, int);
	void onTimer();
	void onItemClicked(QTreeWidgetItem*, int);

private:
	void addComponent(QTreeWidgetItem* parent, const char* szname, int ndata);
	FEModel*	m_fem;
	Data_Tensor_Type	m_class;
	QTreeWidget*	m_tree;
	QTreeWidgetItem*	m_sel;
	bool m_bvec;
	int	m_ntimer;
};
