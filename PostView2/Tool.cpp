#include "Tool.h"
#include "PropertyListForm.h"
#include <QApplication>
#include <QBoxLayout>
#include <QPushButton>

//-----------------------------------------------------------------------------
void CAbstractTool::updateUi()
{
	QWidget* w = QApplication::activeWindow();
	if (w) w->repaint();
}

void CToolUI::hideEvent(QHideEvent* ev)
{
	if (m_tool) m_tool->deactivate();
}

void CToolUI::showEvent(QShowEvent* ev)
{
	if (m_tool) m_tool->activate();
}

//-----------------------------------------------------------------------------
CBasicTool::CBasicTool(const QString& s, CDocument* doc, unsigned int flags) : CAbstractTool(s, doc)
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

	CToolUI* pw = new CToolUI(this);
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
	}
	CAbstractTool::updateUi();
}
