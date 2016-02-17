#pragma once
#include <QComboBox>

class FEModel;
class QTreeWidgetItem;

class CDataFieldSelector : public QComboBox
{
public:
	CDataFieldSelector(QWidget* parent = 0);

	void BuildMenu(FEModel* fem, int ntype, bool btvec = false);

private:
	void addComponent(QTreeWidgetItem* parent, const char* szname, int ndata);
};
