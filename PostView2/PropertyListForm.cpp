#include "PropertyListForm.h"
#include "PropertyList.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include "DataFieldSelector.h"

CPropertyListForm::CPropertyListForm(QWidget* parent) : QWidget(parent)
{
	ui = 0;
	m_list = 0;
}

void CPropertyListForm::setPropertyList(CPropertyList* pl)
{
	if (ui) delete ui;
	m_widget.clear();

	m_list = pl;
	if (pl)
	{
		ui = new QFormLayout(this);
		setLayout(ui);

		int nprops = pl->Properties();
		for (int i=0; i<nprops; ++i)
		{
			const CProperty& pi = pl->Property(i);
			QVariant v = pl->GetPropertyValue(i);
			QWidget* pw = 0;

			if (pi.isEditable())
			{
				switch (pi.type)
				{
				case CProperty::Int:
					{
						QSpinBox* spin = new QSpinBox;
						spin->setRange(pi.imin, pi.imax);
						spin->setValue(v.toInt());
						connect(spin, SIGNAL(valueChanged(int)), this, SLOT(onDataChanged()));
						pw = spin;
					}
					break;
				case CProperty::Float:
					{
/*						QDoubleSpinBox* spin = new QDoubleSpinBox;
						spin->setRange(pi.fmin, pi.fmax);
						spin->setSingleStep(pi.fstep);
						spin->setValue(v.toDouble());
						connect(spin, SIGNAL(valueChanged(double)), this, SLOT(onDataChanged()));
						pw = spin;
*/
						QLineEdit* edit = new QLineEdit; edit->setValidator(new QDoubleValidator);
						edit->setText(QString::number(v.toDouble()));
						connect(edit, SIGNAL(editingFinished()), this, SLOT(onDataChanged()));
						pw = edit;
					}
					break;
				case CProperty::Enum:
					{
						if (pi.values.isEmpty() == false)
						{
							QComboBox* pc = new QComboBox;
							pc->addItems(pi.values);
							pc->setCurrentIndex(v.toInt());
							connect(pc, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataChanged()));
							pw = pc;
						}
					}
					break;
				case CProperty::Bool:
					{
						QComboBox* pc = new QComboBox;
						pc->addItem("No");
						pc->addItem("Yes");
						pc->setCurrentIndex(v.toInt());
						connect(pc, SIGNAL(currentIndexChanged(int)), this, SLOT(onDataChanged()));
						pw = pc;
					}
					break;
				case CProperty::String:
					{
						QLineEdit* edit = new QLineEdit;
						edit->setText(v.toString());
						connect(edit, SIGNAL(textEdited(int)), this, SLOT(onDataChanged()));
						pw = edit;
					}
					break;
				}
			}
			else
			{
				QLineEdit* edit = new QLineEdit;
				edit->setText(v.toString());
				edit->setReadOnly(true);
				QPalette p = edit->palette();
				p.setBrush(QPalette::Base, QColor(236, 236, 236));
				edit->setPalette(p);
				pw = edit;
			}
			if (pw) ui->addRow(pi.name, pw);
			m_widget.push_back(pw);
		}
	}		
}

void CPropertyListForm::updateData()
{
	if (m_list == 0) return;

	int nprops = m_list->Properties();
	QList<QWidget*>::iterator it = m_widget.begin();
	for (int i=0; i<nprops; ++i, ++it)
	{
		QWidget* pw = *it;
		if (pw)
		{
			const CProperty& pi = m_list->Property(i);
			QVariant v = m_list->GetPropertyValue(i);

			if (pi.isEditable())
			{
				switch (pi.type)
				{
				case CProperty::Int:
					{
						QSpinBox* spin = qobject_cast<QSpinBox*>(pw);
						if (spin)
						{
							spin->setRange(pi.imin, pi.imax);
							spin->setValue(v.toInt());
						}
					}
					break;
				case CProperty::Float:
					{
						QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(pw);
						if (spin)
						{
							spin->setRange(pi.fmin, pi.fmax);
							spin->setSingleStep(pi.fstep);
							spin->setValue(v.toDouble());
						}
					}
					break;
				case CProperty::Enum:
					{
						QComboBox* pc = qobject_cast<QComboBox*>(pw);
						if (pc)
						{
							pc->addItems(pi.values);
							pc->setCurrentIndex(v.toInt());
						}
					}
					break;
				case CProperty::Bool:
					{
						QComboBox* pc = qobject_cast<QComboBox*>(pw);
						if (pc) pc->setCurrentIndex(v.toInt());
					}
					break;
				case CProperty::String:
					{
						QLineEdit* edit = qobject_cast<QLineEdit*>(pw);
						if (edit) edit->setText(v.toString());
					}
					break;
				}
			}
			else
			{
				QLineEdit* edit = qobject_cast<QLineEdit*>(pw);
				if (edit) edit->setText(v.toString());
			}
		}
	}
}

void CPropertyListForm::onDataChanged()
{
	if (m_list==0) return;

	QWidget* pw = qobject_cast<QWidget*>(sender());
	if (pw)
	{
		QList<QWidget*>::iterator it = m_widget.begin();
		for (int i=0; i<m_widget.size(); ++i, ++it)
		{
			if (*it==pw)
			{
				const CProperty& pi = m_list->Property(i);
				if (pi.isEditable())
				{
					switch (pi.type)
					{
					case CProperty::Int:
						{
							QSpinBox* spin = qobject_cast<QSpinBox*>(pw);
							if (spin) m_list->SetPropertyValue(i, spin->value());
						}
						break;
					case CProperty::Float:
						{
/*							QDoubleSpinBox* spin = qobject_cast<QDoubleSpinBox*>(pw);
							if (spin) m_list->SetPropertyValue(i, spin->value());
*/
							QLineEdit* edit = qobject_cast<QLineEdit*>(pw);
							if (edit) m_list->SetPropertyValue(i, edit->text().toDouble());
						}
						break;
					case CProperty::Enum:
						{
							QComboBox* pc = qobject_cast<QComboBox*>(pw);
							if (pc) m_list->SetPropertyValue(i, pc->currentIndex());
						}
						break;
					case CProperty::Bool:
						{
							QComboBox* pc = qobject_cast<QComboBox*>(pw);
							if (pc) m_list->SetPropertyValue(i, pc->currentIndex());
						}
						break;
					case CProperty::String:
						{
							QLineEdit* edit = qobject_cast<QLineEdit*>(pw);
							if (edit) m_list->SetPropertyValue(i, edit->text());
						}
						break;
					case CProperty::DataScalar:
						{
							CDataFieldSelector* pc = dynamic_cast<CDataFieldSelector*>(pw);
							if (pc) m_list->SetPropertyValue(i, pc->currentData(Qt::UserRole));
						}
						break;
					case CProperty::DataVec3:
						{
							CDataFieldSelector* pc = dynamic_cast<CDataFieldSelector*>(pw);
							if (pc) m_list->SetPropertyValue(i, pc->currentData(Qt::UserRole));
						}
						break;
					}
				}
				return;
			}
		}
	}
}
