#include "PropertyListView.h"
#include <QBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QLabel>
#include <QComboBox>
#include <QApplication>

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
			pitem->setText(p.m_val);
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
		m_prop->setCellWidget(m_selRow, m_selCol, 0);
		m_sel = 0;
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
		m_prop->setCellWidget(m_selRow, m_selCol, 0);
		m_sel = 0;
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
	if (column == 1)
	{
		QVariant v = m_list->GetPropertyValue(row);

		switch (v.type())
		{
		case QVariant::Bool:
			{
				QComboBox* pc = new QComboBox(m_prop);
				pc->addItem("No");
				pc->addItem("Yes");
				pc->setCurrentIndex(v.value<bool>() ? 1 : 0);
				connect(pc, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged(int)));

				m_sel = pc;
				m_selRow = row;
				m_selCol = column;

				m_prop->setCellWidget(row, column, pc);
			}
		}
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

		m_prop->item(m_selRow, 1)->setText(b ? "Yes" : "No");

		QApplication::activeWindow()->repaint();
	}
}
