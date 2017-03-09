#include "DataFieldSelector.h"
#include <QTreeWidget>
#include <QHeaderView>
#include <PostViewLib/FEModel.h>
#include <PostViewLib/constants.h>
#include <QtCore/QTimer>

CDataFieldSelector::CDataFieldSelector(QWidget* parent) : QComboBox(parent)
{
	m_tree = new QTreeWidget;
	m_tree->header()->hide();
	m_tree->setMouseTracking(true);
	setModel(m_tree->model());
	setView(m_tree);
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
	m_fem = 0;

	m_sel = 0;
	m_ntimer = 0;

	setMaxVisibleItems(50);

	QObject::connect(m_tree, SIGNAL(itemEntered(QTreeWidgetItem*,int)), this, SLOT(onItemEntered(QTreeWidgetItem*,int)));
}

CDataFieldSelector::~CDataFieldSelector()
{
	if (m_fem) m_fem->RemoveDependant(this);
}

void CDataFieldSelector::onTimer()
{
	m_ntimer--;
	if (m_ntimer == 0)
	{
		if (m_sel && (m_sel->isExpanded() == false)) m_sel->setExpanded(true);
		this->view()->updateGeometry();
		m_sel = 0;
	}
}

void CDataFieldSelector::onItemEntered(QTreeWidgetItem* item, int)
{
	m_ntimer++;
	m_sel = item;
	QTimer::singleShot(500, this, SLOT(onTimer()));
}

void CDataFieldSelector::BuildMenu(FEModel* fem, Data_Tensor_Type nclass, bool btvec)
{
	if (m_fem != fem)
	{
		m_fem = fem;
		if (fem) fem->AddDependant(this);
	}

	m_class = nclass;
	m_bvec = btvec;

	// get the current field
	int noldField = currentValue();

	// get the tree view and clear it
	QTreeWidget* pw = qobject_cast<QTreeWidget*>(view());
	pw->clear();

	// get the datamanager
	if (fem == 0) return;

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
				pi->setExpanded(false);

				QFont font = pi->font(0);
				font.setBold(true);
				pi->setFont(0, font);

				for (int n=0; n<dataComponents; ++n)
				{
					int nfield = BUILD_FIELD(dataClass, i, n);
					std::string s = d.componentName(n, nclass);
					addComponent(pi, s.c_str(), nfield);
				}
			}
		}
	}

	// reset the current selection
	setCurrentValue(noldField);
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
	if (nfield == -1) { setCurrentIndex(-1); return; }

	int ncode = FIELD_CODE(nfield);
	int ncomp = FIELD_COMP(nfield);

	QTreeWidgetItemIterator it(m_tree);
	while (*it)
	{
		int nitemValue = (*it)->data(0, Qt::UserRole).toInt();
		if (nitemValue == nfield)
		{
			if ((*it)->parent() == 0)
			{
				// we must reset the current index, since if ncode is the current index 
				// this won't trigger an update of the FEModel resulting in a gargabe state.
				setCurrentIndex(-1);

				// now set the current index
				setCurrentIndex(ncode);
			}
			else
			{
				m_tree->setCurrentItem((*it)->parent(), 0);
				setRootModelIndex(m_tree->currentIndex());
				setCurrentIndex(ncomp);
				m_tree->setCurrentItem(m_tree->invisibleRootItem(), 0);
				setRootModelIndex(m_tree->currentIndex());
			}
			return;
		}

		++it;
	}

	setCurrentIndex(-1);
}

void CDataFieldSelector::Update(FEModel* pfem)
{
	if (m_fem) BuildMenu(pfem, m_class, m_bvec);
}
