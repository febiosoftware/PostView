#include "DataFieldSelector.h"
#include <QTreeWidget>
#include <QHeaderView>
#include <PostViewLib/FEModel.h>
#include <PostViewLib/constants.h>

CDataFieldSelector::CDataFieldSelector(QWidget* parent) : QComboBox(parent)
{
	QTreeWidget* pw = new QTreeWidget;
	pw->header()->hide();
	setModel(pw->model());
	setView(pw);
//	setMinimumWidth(200);
	view()->setMinimumWidth(200);
	view()->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	setCurrentIndex(-1);

/*    QString styleSheet = "QComboBox \
        {  \
            background-color:           white; \
        }";

    setStyleSheet(styleSheet); 
*/
}

void CDataFieldSelector::BuildMenu(FEModel* fem, Data_Tensor_Type nclass, bool btvec)
{
	// get the tree view and clear it
	QTreeWidget* pw = qobject_cast<QTreeWidget*>(view());
	pw->clear();

	// get the datamanager
	FEDataManager& dm = *fem->GetDataManager();

	QTreeWidgetItem* pi;

	// loop over all data fields
	int N = dm.DataFields();
	FEDataFieldPtr pd = dm.FirstDataField();
	for (int i=0; i<N; ++i, ++pd)
	{
		FEDataField& d = *(*pd);
		int dataClass = d.DataClass();
		int dataComponents = d.components(nclass);
		if (dataComponents > 0)
		{
			if (dataComponents == 1)
			{
				int nfield = BUILD_FIELD(dataClass, i, 0);
				pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
				pi->setData(0, Qt::UserRole, QVariant(nfield));
			}
			else
			{
				pi = new QTreeWidgetItem(pw); pi->setText(0, d.GetName());
				pi->setFlags(pi->flags() & ~Qt::ItemIsSelectable); 
				pi->setExpanded(true);

				for (int n=0; n<dataComponents; ++n)
				{
					int nfield = BUILD_FIELD(dataClass, i, n);
					std::string s = d.componentName(n, nclass);
					addComponent(pi, s.c_str(), nfield);
				}
			}
		}
	}

	// clear the current selection
	setCurrentIndex(-1);
}

void CDataFieldSelector::addComponent(QTreeWidgetItem* parent, const char* szname, int ndata)
{
	QTreeWidgetItem* pi = new QTreeWidgetItem(parent);
	pi->setText(0, szname);
	pi->setData(0, Qt::UserRole, QVariant(ndata));
}

int CDataFieldSelector::currentValue() const
{
	int index = currentIndex();
	if (index >= 0) return currentData(Qt::UserRole).toInt();
	return -1;
}

void CDataFieldSelector::setCurrentValue(int nfield)
{
	int n = count();
	for (int i=0; i<n; ++i)
	{
		int nfield = itemData(i, Qt::UserRole).toInt();
	}

/*	if (IS_VALID(nfield))
	{
		int ndata = FIELD_CODE(nfield);
		int ncomp = FIELD_COMP(nfield);
		QTreeWidget* pw = qobject_cast<QTreeWidget*>(view());
		QTreeWidgetItemIterator it(pw);
		while (*it)
		{
			if ((*it)->data(0, Qt::UserRole) == nfield)
			{
				pw->setCurrentItem(*it);
				break;
			}
			++it;
		}
	}
	else setCurrentIndex(-1);
*/
}
