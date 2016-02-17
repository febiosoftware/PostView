#include "Tool.h"
#include "PropertyListForm.h"
#include <QApplication>

//-----------------------------------------------------------------------------
CBasicTool::CBasicTool(const QString& s) : CAbstractTool(s)
{
	m_list = 0;
	m_form = 0;
}

//-----------------------------------------------------------------------------
QWidget* CBasicTool::createUi()
{
	m_list = getPropertyList();
	if (m_list == 0) return 0;

	m_form = new CPropertyListForm;
	m_form->setPropertyList(m_list);

	return m_form;
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
