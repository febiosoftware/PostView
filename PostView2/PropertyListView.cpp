#include "PropertyListView.h"
#include <QBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>
#include <QComboBox>
#include <QApplication>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>

//-----------------------------------------------------------------------------
class CIntInput : public QLineEdit
{
public:
	CIntInput(QWidget* parent = 0) : QLineEdit(parent)
	{
		setValidator(new QIntValidator);
	}

	void setValue(int m) { setText(QString("%1").arg(m)); }
	int value() const { return text().toInt(); }
};

//-----------------------------------------------------------------------------
class CFloatInput : public QLineEdit
{
public:
	CFloatInput(QWidget* parent = 0) : QLineEdit(parent)
	{
		QDoubleValidator* pv = new QDoubleValidator;
		pv->setRange(-1e99, 1e99, 3);
		setValidator(pv);
	}

	void setValue(double v) { setText(QString("%1").arg(v)); }
	double value() const { return text().toDouble(); }
};

//-----------------------------------------------------------------------------
CPropertyListView::CPropertyListView(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* playout = new QVBoxLayout(this);
	setLayout(playout);
	playout->setMargin(0);

	m_prop = new QTableWidget;
	m_prop->setObjectName(QStringLiteral("modelProps"));

	m_info = new QLabel;
	m_info->setFrameStyle(QFrame::Panel);
	m_info->setMinimumHeight(50);
		
	playout->addWidget(m_prop);
	playout->addWidget(m_info);

	m_list = 0;
	m_sel = 0;
	m_selRow = -1;

	QMetaObject::connectSlotsByName(this);
}

//-----------------------------------------------------------------------------
void CPropertyListView::Update(CPropertyList* plist)
{
	if (m_list) Clear();
	m_list = plist;

	int n = plist->Properties();
	if (n > 0)
	{
		m_prop->setRowCount(n);
		m_prop->horizontalHeader()->setDefaultSectionSize(width()/2);
		for (int i=0; i<n; ++i)
		{
			const CPropertyList::CProperty& p = plist->Property(i);

			QTableWidgetItem* pitem = new QTableWidgetItem;
			pitem->setFlags(pitem->flags() & ~Qt::ItemIsEditable);
			pitem->setText(p.m_name);

			QFont f = pitem->font(); f.setBold(true);
			pitem->setFont(f);
			m_prop->setItem(i, 0, pitem);

			pitem = new QTableWidgetItem;
			pitem->setFlags(pitem->flags() & ~Qt::ItemIsEditable);
			QVariant v = plist->GetPropertyValue(i);
			switch (p.m_type)
			{
			case QVariant::Color: pitem->setBackgroundColor(v.value<QColor>()); break;
			case QVariant::Bool:  pitem->setText(v.value<bool>() ? "Yes" : "No"); break;
			case QVariant::Int:
				if (p.m_values.isEmpty()) pitem->setText(v.toString());
				else pitem->setText(p.m_values.at(v.value<int>()));
				break;
			default:
				pitem->setText(v.toString());
			}
			m_prop->setItem(i, 1, pitem);
		}
	}
}

//-----------------------------------------------------------------------------
void CPropertyListView::Clear()
{
	m_list = 0;

	if (m_sel)
	{
		m_prop->setCellWidget(m_selRow, 1, 0);
		m_sel = 0; m_selRow = -1;
	}

	m_prop->clear();
	m_prop->setColumnCount(2);
	m_prop->setRowCount(0);
	m_prop->horizontalHeader()->setStretchLastSection(true);
	m_prop->horizontalHeader()->hide();
	m_prop->horizontalHeader()->setDefaultSectionSize(width()/2);
	m_prop->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	m_prop->verticalHeader()->setDefaultSectionSize(24);
	m_prop->verticalHeader()->hide();
	m_prop->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_prop->setSelectionMode(QAbstractItemView::SingleSelection);

	m_info->clear();
}

//-----------------------------------------------------------------------------
void CPropertyListView::on_modelProps_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (m_sel)
	{
		if (currentRow == m_selRow) return;

		m_prop->setCellWidget(m_selRow, 1, 0);
		m_sel = 0; m_selRow = -1;
	}

	if (m_list)
	{
		if ((currentRow >= 0)&&(currentRow < m_list->Properties()))
		{
			const CPropertyList::CProperty& pi = m_list->Property(currentRow);
			m_info->setText(tr("<p><b>%1</b></p><p>%2</p>").arg(pi.m_name).arg(pi.m_info));
			return;
		}
	}
	m_info->clear();
}

//-----------------------------------------------------------------------------
void CPropertyListView::on_modelProps_cellClicked(int row, int column)
{
	if (row == m_selRow) return;

	QVariant v = m_list->GetPropertyValue(row);

	QWidget* pw = 0;
	switch (v.type())
	{
	case QVariant::Bool:
		{
			QComboBox* pc = new QComboBox(m_prop);
			pc->addItem("No");
			pc->addItem("Yes");
			pc->setCurrentIndex(v.value<bool>() ? 1 : 0);
			connect(pc, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
			pw = pc;
		}
		break;
	case QVariant::Color:
		{
			CColorButton* pc = new CColorButton(m_prop);
			pc->setColor(v.value<QColor>());
			connect(pc, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
			pw = pc;
		}
		break;
	case QVariant::Int:
		{
			const CPropertyList::CProperty& p = m_list->Property(row);
			if (p.m_values.isEmpty())
			{
				CIntInput* pi = new CIntInput(m_prop);
				pi->setValue(v.value<int>());
				connect(pi, SIGNAL(textEdited(const QString&)), this, SLOT(intChanged(const QString&)));
				pw = pi;
			}
			else
			{
				QComboBox* pc = new QComboBox(m_prop);
				for (int i=0; i<p.m_values.size(); ++i) pc->addItem(p.m_values.at(i));
				pc->setCurrentIndex(v.value<int>());
				connect(pc, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));
				pw = pc;
			}
		}
		break;
	case QVariant::Double:
		{
			CFloatInput* pf = new CFloatInput(m_prop);
			pf->setValue(v.value<double>());
			connect(pf, SIGNAL(textEdited(const QString&)), this, SLOT(floatChanged(const QString&)));
			pw = pf;
		}
		break;
	}

	if (pw)
	{
		m_sel = pw;
		m_selRow = row;
		m_prop->setCellWidget(row, 1, pw);
	}
}

//-----------------------------------------------------------------------------
void CPropertyListView::comboChanged(int val)
{
	if (m_sel)
	{
		bool b = (val==0 ? false : true);
		QVariant v = b;

		m_list->SetPropertyValue(m_selRow, v);
		const CPropertyList::CProperty& p = m_list->Property(m_selRow);

		if (p.m_type == QVariant::Bool)
		{
			m_prop->item(m_selRow, 1)->setText(b ? "Yes" : "No");
		}
		else
		{
			m_prop->item(m_selRow, 1)->setText(p.m_values.at(val));
		}

		QApplication::activeWindow()->repaint();
	}
}

//-----------------------------------------------------------------------------
void CPropertyListView::colorChanged(QColor c)
{
	if (m_sel)
	{
		QVariant v = c;
		m_list->SetPropertyValue(m_selRow, v);
		m_prop->item(m_selRow, 1)->setBackgroundColor(c);
		QApplication::activeWindow()->repaint();
	}
}

//-----------------------------------------------------------------------------
void CPropertyListView::intChanged(const QString& s)
{
	if (m_sel)
	{
		QVariant v = s.toInt();
		m_list->SetPropertyValue(m_selRow, v);
		m_prop->item(m_selRow, 1)->setText(s);
		QApplication::activeWindow()->repaint();
	}
}

//-----------------------------------------------------------------------------
void CPropertyListView::floatChanged(const QString& s)
{
	if (m_sel)
	{
		QVariant v = s.toDouble();
		m_list->SetPropertyValue(m_selRow, v);
		m_prop->item(m_selRow, 1)->setText(s);
		QApplication::activeWindow()->repaint();
	}
}
