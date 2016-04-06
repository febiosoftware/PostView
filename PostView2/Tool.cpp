#include "Tool.h"
#include "PropertyListForm.h"
#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>

//-----------------------------------------------------------------------------
CBasicTool::CBasicTool(const QString& s, unsigned int flags) : CAbstractTool(s)
{
	m_list = 0;
	m_form = 0;
	m_flags = flags;
}

//-----------------------------------------------------------------------------
QWidget* CBasicTool::createUi()
{
	m_list = getPropertyList();
	if (m_list == 0) return 0;

	QWidget* pw = new QWidget;
	QVBoxLayout* pl = new QVBoxLayout(pw);
	pw->setLayout(pl);

	m_form = new CPropertyListForm;
	m_form->setPropertyList(m_list);
	pl->addWidget(m_form);

	if (m_flags & HAS_APPLY_BUTTON)
	{
		QHBoxLayout* pg = new QHBoxLayout;
		pg->addStretch();
		
		QPushButton* pb = new QPushButton("Apply");
		pg->addWidget(pb);
		pl->addLayout(pg);

		QObject::connect(pb, SIGNAL(clicked(bool)), this, SLOT(on_button_clicked()));
	}

	return pw;
}

//-----------------------------------------------------------------------------
void CBasicTool::on_button_clicked()
{
	OnApply();
}

//-----------------------------------------------------------------------------
void CBasicTool::updateUi()
{
	if (m_form) 
	{
		m_form->updateData();
		QApplication::activeWindow()->repaint();
	}
}
