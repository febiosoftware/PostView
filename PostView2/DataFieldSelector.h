#pragma once
#include <QPushButton>
#include <QMenu>
#include <PostViewLib/FEMeshData.h>
#include <PostViewLib/FEModel.h>

class CDataFieldSelector : public QPushButton, public FEModelDependant
{
	Q_OBJECT

public:
	CDataFieldSelector(QWidget* parent = 0);
	~CDataFieldSelector();

	// build the menu
	void BuildMenu(FEModel* fem, Data_Tensor_Type ntype, bool btvec = false);

	// return the field ID of the selected menu
	int currentValue() const;

	// set the current field ID
	void setCurrentValue(int n);

public:
	// inherited from FEModelDependant
	void Update(FEModel* pfem);

protected slots:
	void onAction(QAction* pa);

signals:
	void currentValueChanged(int n);

private:
	FEModel*			m_fem;
	Data_Tensor_Type	m_class;
	bool m_bvec;
	int	m_ntimer;

private:
	QMenu*	m_menu;
	int		m_currentValue;
};
