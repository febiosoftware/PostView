#include "DataFieldSelector.h"
#include <PostViewLib/FEModel.h>
#include <PostViewLib/constants.h>
#include <QPainter>

CDataFieldSelector::CDataFieldSelector(QWidget* parent) : QPushButton(parent)
{
	m_menu = new QMenu(this);
	m_currentValue = -1;
	m_fem = 0;

	setStyleSheet("QPushButton { text-align : left; }");

	setMenu(m_menu);

	QObject::connect(m_menu, SIGNAL(triggered(QAction*)), this, SLOT(onAction(QAction*)));
}

CDataFieldSelector::~CDataFieldSelector()
{
	if (m_fem) m_fem->RemoveDependant(this);
}

void CDataFieldSelector::BuildMenu(FEModel* fem, Data_Tensor_Type nclass, bool btvec)
{
	// make sure the field selector is a dependant of the model 
	if (m_fem != fem)
	{
		m_fem = fem;
		if (fem) fem->AddDependant(this);
	}

	// store the parameters
	m_class = nclass;
	m_bvec = btvec;

	// get the current field
	// we'll use it to restore the current selected option
	int noldField = m_currentValue;
	m_currentValue = -1;
	setText("");

	// get the datamanager
	if (fem == 0) return;
	FEDataManager& dm = *fem->GetDataManager();

	// clear the menu
	m_menu->clear();

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
			if ((dataComponents == 1) && (d.Type() != DATA_ARRAY))
			{
				int nfield = BUILD_FIELD(dataClass, i, 0);

				QAction* pa = m_menu->addAction(QString::fromStdString(d.GetName()));
				pa->setData(QVariant(nfield));

				if (nfield == noldField)
				{
					setText(pa->text());
					m_currentValue = noldField;
				}
			}
			else
			{
				QMenu* sub = new QMenu(QString::fromStdString(d.GetName()), m_menu);
				m_menu->addMenu(sub);

				for (int n=0; n<dataComponents; ++n)
				{
					int nfield = BUILD_FIELD(dataClass, i, n);
					std::string s = d.componentName(n, nclass, true);

					QAction* pa = sub->addAction(QString::fromStdString(s));
					pa->setData(QVariant(nfield));

					if (nfield == noldField)
					{
						setText(pa->text());
						m_currentValue = noldField;
					}
				}
			}
		}
	}

	// if the old field was not found, send out a signal
	if (m_currentValue == -1)
		emit currentValueChanged(m_currentValue);
}

int CDataFieldSelector::currentValue() const
{
	return m_currentValue;
}

void CDataFieldSelector::setCurrentValue(int newField)
{
	// make sure there is something to change
	if (newField == m_currentValue) return;

	if (m_fem)
	{
		string fieldName;
		FEDataManager& dm = *m_fem->GetDataManager();
		int N = dm.DataFields();
		FEDataFieldPtr pd = dm.FirstDataField();
		for (int i = 0; i<N; ++i, ++pd)
		{
			FEDataField& d = *(*pd);
			int dataClass = d.DataClass();
			int dataComponents = d.components(m_class);
			if (dataComponents > 0)
			{
				if (dataComponents == 1)
				{
					int nfield = BUILD_FIELD(dataClass, i, 0);
					if (nfield == newField)
					{
						fieldName = d.GetName();
						break;
					}
				}
				else
				{
					for (int n = 0; n<dataComponents; ++n)
					{
						int nfield = BUILD_FIELD(dataClass, i, n);
						if (nfield == newField)
						{
							fieldName = d.componentName(n, m_class);
							break;
						}
					}
				}
			}
		}

		if (fieldName.empty() == false)
		{
			m_currentValue = newField;
			setText(QString::fromStdString(fieldName));
		}
		else
		{
			m_currentValue = -1;
			setText("");
		}

		emit currentValueChanged(m_currentValue);
	}
}

void CDataFieldSelector::Update(FEModel* pfem)
{
	BuildMenu(pfem, m_class, m_bvec);
}

void CDataFieldSelector::onAction(QAction* pa)
{
	setText(pa->text());
	m_currentValue = pa->data().toInt();

	emit currentValueChanged(m_currentValue);
}
